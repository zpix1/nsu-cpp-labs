#include "Worker.h"
#include "utility.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <regex>

using namespace Workflow;

void ReadfileWorker::run_operation(const ArgumentList& arguments, Context& context) {
    readfile(arguments[0], context.text);
}

void WritefileWorker::run_operation(const ArgumentList& arguments, Context& context) {
    writefile(arguments[0], context.text);
}

void GrepWorker::run_operation(const ArgumentList& arguments, Context& context) {
    auto egrep_regex = std::regex(arguments[0]);
    context.text.erase(std::remove_if(context.text.begin(), context.text.end(), [egrep_regex](const auto& str) {
        return !std::regex_search(str, egrep_regex);
    }), context.text.end());
}

void SortWorker::run_operation(const ArgumentList& arguments, Context& context) {
    std::sort(context.text.begin(), context.text.end());
}

auto replace_all(std::string& str, const std::string& replace_from, const std::string& replace_to) {
    return std::regex_replace(str, std::regex(replace_from), replace_to);
}

void ReplaceWorker::run_operation(const ArgumentList& arguments, Context& context) {
    auto replace_from = arguments[0];
    auto replace_to = arguments[1];
    for (std::string& str: context.text) {
        // std::regex_replace doesn't support inplace replacing
        str = replace_all(str, replace_from, replace_to);
    }
}

void DumpWorker::run_operation(const ArgumentList& arguments, Context& context) {
    auto output = std::ofstream(arguments[0]);
    for (const auto& str: context.text) {
        output << str << std::endl;
    }
}

auto string_split(std::string str, std::string_view delimiter) {
    std::vector<std::string> result;
    std::size_t pos;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    result.push_back(str);
    return result;
}

std::unique_ptr<Worker> get_worker_by_name(std::string_view name) {
    if (name == "readfile")
        return std::make_unique<ReadfileWorker>(ReadfileWorker());
    if (name == "writefile")
        return std::make_unique<WritefileWorker>(WritefileWorker());
    if (name == "grep")
        return std::make_unique<GrepWorker>(GrepWorker());
    if (name == "sort")
        return std::make_unique<SortWorker>(SortWorker());
    if (name == "replace")
        return std::make_unique<ReplaceWorker>(ReplaceWorker());
    if (name == "dump")
        return std::make_unique<DumpWorker>(DumpWorker());
    return nullptr;
}

// std::stoi converts strings with floating point numbers (like 4.0 or 2.3) without exceptions
auto str_to_worker_id(const std::string& str_id, int nline) {
    WorkerID id;
    try {
        id = std::stoi(str_id);
    } catch (std::invalid_argument& e) {
        throw ParsingException("non-negative integer id expected, invalid id found", nline);
    } catch (std::out_of_range& e) {
        throw ParsingException("non-negative integer id expected, out-of-range id found", nline);
    }

    if (id < 0) {
        throw ParsingException("non-negative integer id expected, negative id found", nline);
    }

    return id;
}

Scheme WorkflowExecutor::parse(const TextContainer& text) const {
    const std::string delimiter = " = ";

    // First line show be desc
    if (text[0] != "desc") throw ParsingException("desc tag in the first line expected, but not found", 0 + 1);

    Scheme scheme;
    auto i = 1;
    for (; text[i] != "csed"; i++) {
        auto pos = text[i].find(delimiter);
        if (pos == std::string::npos) throw ParsingException("id-worker delimiter expected, but not found", i + 1);

        auto id = str_to_worker_id(text[i].substr(0, pos), i + 1);
        auto worker_with_arguments = text[i].substr(pos + delimiter.length(), text[i].length());

        auto arguments = static_cast<ArgumentList>(string_split(worker_with_arguments, " "));
        if (arguments.empty()) throw ParsingException("worker name expected, but not found", i + 1);

        auto worker_ptr = get_worker_by_name(arguments[0]);
        if (worker_ptr == nullptr) throw ParsingException("invalid worker name", i + 1);

        arguments.erase(arguments.begin());

        if (scheme.id2worker.count(id) != 0) {
            throw ParsingException("unique id expected, but a repetition found", i + 1);
        }

        scheme.id2worker.insert(std::make_pair(id,
                                               std::make_pair(std::move(worker_ptr), std::move(arguments))
        ));

        // csed should appear
        if (i + 1 == text.size()) throw ParsingException("csed expected, but not found", i + 1);
    }
    i++;

    // Empty lines are allowed in FlagIO mode
    if (i == text.size()) return scheme;

    if (i != text.size() - 1) throw ParsingException("flow line is expected to be the last", i + 1);

    auto flow_tokens = string_split(text[i], " -> ");
    if (flow_tokens == std::vector<std::string>{""}) return scheme;
    for (const auto& s: flow_tokens) {
        auto id = str_to_worker_id(s, i + 1);
        if (scheme.id2worker.count(id) == 0) throw ParsingException("undeclared id found", i + 1);
        scheme.execution_flow.push_back(id);
    }

    return scheme;
}

// dynamic_case can be changed to checking working property (with adding an additional method)
void WorkflowExecutor::validate(const Scheme& scheme, InputOutputMode mode) const {
    // Check arguments for each worker
    for (const auto&[id, worker_with_arguments]: scheme.id2worker) {
        if (!worker_with_arguments.first->check_arguments(worker_with_arguments.second)) {
            throw ValidationException("invalid arguments for worker",
                                      ValidationException::ErrorDomain::Description,
                                      id + 1);
        }
    }
    if (mode == InputOutputMode::None) {
        return;
    }
    if (mode == InputOutputMode::FlagZero) {
        if (scheme.execution_flow.size() < 2) {
            throw ValidationException("not enough flow elements for file mode",
                                      ValidationException::ErrorDomain::ExecutionFlow, 1);
        }
    } else if (mode == InputOutputMode::FlagI || mode == InputOutputMode::FlagO) {
        if (scheme.execution_flow.empty()) {
            throw ValidationException("not enough flow elements for file mode",
                                      ValidationException::ErrorDomain::ExecutionFlow, 1);
        }
    }
    // Check start read
    if (mode == InputOutputMode::FlagO || mode == InputOutputMode::FlagZero) {
        if ((dynamic_cast<ReadfileWorker *>(scheme.id2worker.at(scheme.execution_flow[0]).first.get()) == nullptr)) {
            throw ValidationException("read file worker does not present, but expected",
                                      ValidationException::ErrorDomain::ExecutionFlow, 1);
        }
    }
    // Check end write
    if (mode == InputOutputMode::FlagI || mode == InputOutputMode::FlagZero) {
        if ((dynamic_cast<WritefileWorker *>(scheme.id2worker.at(
                scheme.execution_flow[scheme.execution_flow.size() - 1]).first.get()) == nullptr)) {
            throw ValidationException("write file worker does not present, but expected",
                                      ValidationException::ErrorDomain::ExecutionFlow, scheme.execution_flow.size());
        }
    }
    auto start = mode == InputOutputMode::FlagI || mode == InputOutputMode::FlagIO ? 0 : 1;
    auto end = mode == InputOutputMode::FlagO || mode == InputOutputMode::FlagIO ? scheme.execution_flow.size() :
               scheme.execution_flow.size() - 1;

    // Bad read/write workers check
    for (auto i = start; i < end; i++) {
        if ((dynamic_cast<ReadfileWorker *>(scheme.id2worker.at(scheme.execution_flow[i]).first.get()) !=
             nullptr) ||
            ((dynamic_cast<WritefileWorker *>(scheme.id2worker.at(scheme.execution_flow[i]).first.get()) !=
              nullptr))) {
            throw ValidationException("read or write worker unexpected in command line files indication mode",
                                      ValidationException::ErrorDomain::ExecutionFlow,
                                      i + 1);
        }
    }
}

void WorkflowExecutor::execute_with_ctx(const Scheme& scheme, Context& ctx) {
    for (const auto id: scheme.execution_flow) {
        // Why [] does not work, but .at() does?
        // ANSWER: _____
        const auto& worker_with_arguments = scheme.id2worker.at(id);
        worker_with_arguments.first->run_operation(worker_with_arguments.second, ctx);
    }
}

void WorkflowExecutor::execute(const Scheme& scheme) {
    Context ctx;
    execute_with_ctx(scheme, ctx);
}

void WorkflowExecutor::execute(const Scheme& scheme, std::istream& input) {
    Context ctx;
    readfile(input, ctx.text);
    execute_with_ctx(scheme, ctx);
}

void WorkflowExecutor::execute(const Scheme& scheme, std::ostream& output) {
    Context ctx;
    execute_with_ctx(scheme, ctx);
    writefile(output, ctx.text);
}

void WorkflowExecutor::execute(const Scheme& scheme, std::istream& input, std::ostream& output) {
    Context ctx;
    readfile(input, ctx.text);
    execute_with_ctx(scheme, ctx);
    writefile(output, ctx.text);
}

