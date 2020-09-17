#include "Worker.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <charconv>
#include <sstream>

using namespace Workflow;

void ReadfileWorker::run_operation(Context &context) {
    auto input = std::ifstream(context.arguments[0]);
    std::string str;
    while (std::getline(input, str)) {
        context.text.push_back(str);
    }
}

void WritefileWorker::run_operation(Context &context) {
    auto output = std::ofstream(context.arguments[0]);
    for (const auto &str: context.text) {
        output << str << std::endl;
    }
}

void GrepWorker::run_operation(Context &context) {
    auto grepstr = context.arguments[0];
    TextContainer new_text;
    for (const auto &str: context.text) {
        if (str.find(grepstr) != std::string::npos) {
            new_text.push_back(str);
        }
    }
    context.text = new_text;
}

void SortWorker::run_operation(Context &context) {
    std::sort(context.text.begin(), context.text.end());
}

// !TODO: replace it with regex way
void replace_all(std::string &str, std::string_view replace_from, std::string_view replace_to) {
    std::size_t pos = str.find(replace_from);
    while (pos != std::string::npos) {
        str.replace(pos, replace_from.length(), replace_to);
        pos = str.find(replace_from);
    }
}

void ReplaceWorker::run_operation(Context &context) {
    auto replace_from = context.arguments[0];
    auto replace_to = context.arguments[1];
    for (auto &str: context.text) {
        replace_all(str, replace_from, replace_to);
    }
}

void DumpWorker::run_operation(Context &context) {
    auto output = std::ofstream(context.arguments[0]);
    for (const auto &str: context.text) {
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

auto str_to_worker_id(const std::string &str_id, int nline) {
    WorkerID id;
    try {
        id = std::stoi(str_id);
    } catch (std::invalid_argument &e) {
        throw ParsingException("non-negative integer id expected, invalid id found", nline);
    } catch (std::out_of_range &e) {
        throw ParsingException("non-negative integer id expected, out-of-range id found", nline);
    }

    if (id < 0) {
        throw ParsingException("non-negative integer id expected, negative integer id found", nline);
    }

    return id;
}

Scheme WorkflowExecutor::parse(const TextContainer &text) const {
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

    if (i == text.size()) return scheme;

    if (i != text.size() - 1) throw ParsingException("flow line is expected to be the last", i + 1);

    auto flow_tokens = string_split(text[i], " -> ");
    for (const auto &s: flow_tokens) {
        scheme.execution_flow.push_back(str_to_worker_id(s, i + 1));
    }

    return scheme;
}

bool WorkflowExecutor::validate(const Scheme &scheme, const InputOutputMode mode) const {
    // check arguments for each worker
    for (const auto&[id, worker_with_arguments]: scheme.id2worker) {
        if (!worker_with_arguments.first->check_arguments(worker_with_arguments.second)) {
            throw ValidationException("invalid arguments for worker",
                                      ValidationException::ErrorDomain::Description,
                                      id + 1);
        }
    }
    // check flow
    if (mode == InputOutputMode::None) {
        return true;
    }
    if (mode == InputOutputMode::FileMode) {
        if (scheme.execution_flow.size() < 2) {
            throw ValidationException("not enough flow elements for file mode",
                                      ValidationException::ErrorDomain::ExecutionFlow, 1);
        }
        if ((dynamic_cast<ReadfileWorker *>(scheme.id2worker.at(scheme.execution_flow[0]).first.get()) == nullptr)) {
            throw ValidationException("read file worker does not present, but expected",
                                      ValidationException::ErrorDomain::ExecutionFlow, 1);
        }
        if ((dynamic_cast<WritefileWorker *>(scheme.id2worker.at(
                scheme.execution_flow[scheme.execution_flow.size() - 1]).first.get()) == nullptr)) {
            throw ValidationException("write file worker does not present, but expected",
                                      ValidationException::ErrorDomain::ExecutionFlow, 1);
        }
    } else if (mode == InputOutputMode::CmdlineFlagMode) {
        for (auto i = 0; i < scheme.execution_flow.size(); i++) {
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
    return true;
}

InputOutputMode WorkflowExecutor::check_mode(const Scheme &scheme) const {
    if (scheme.execution_flow.empty()) return InputOutputMode::None;
    if ((dynamic_cast<ReadfileWorker *>(scheme.id2worker.at(scheme.execution_flow[0]).first.get()) != nullptr)
        && (dynamic_cast<WritefileWorker *>(scheme.id2worker.at(
            scheme.execution_flow[scheme.execution_flow.size() - 1]).first.get()) != nullptr)) {
        return InputOutputMode::FileMode;
    }
    return InputOutputMode::CmdlineFlagMode;
}

void WorkflowExecutor::execute(const Scheme &scheme, std::ifstream input, std::ofstream output) {

}

void WorkflowExecutor::execute(const Scheme &scheme) {

}
