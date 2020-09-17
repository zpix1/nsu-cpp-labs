#include "Worker.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <regex>

using namespace Workflow;

void ReadfileWorker::run_operation(Context& context) {
    auto input = std::ifstream(context.arguments[0]);
    std::string str;
    while (std::getline(input, str)) {
        context.text.push_back(str);
    }
}

void WritefileWorker::run_operation(Context& context) {
    auto output = std::ofstream(context.arguments[0]);
    for (const auto& str: context.text) {
        output << str << std::endl;
    }
}

void GrepWorker::run_operation(Context& context) {
    auto grepstr = context.arguments[0];
    TextContainer new_text;
    for (const auto& str: context.text) {
        if (str.find(grepstr) != std::string::npos) {
            new_text.push_back(str);
        }
    }
    context.text = new_text;
}

void SortWorker::run_operation(Context& context) {
    std::sort(context.text.begin(), context.text.end());
}

// !TODO: replace it with regex way
void replace_all(std::string& str, std::string_view replace_from, std::string_view replace_to) {
    std::size_t pos = str.find(replace_from);
    while (pos != std::string::npos) {
        str.replace(pos, replace_from.length(), replace_to);
        pos = str.find(replace_from);
    }
}

void ReplaceWorker::run_operation(Context& context) {
    auto replace_from = context.arguments[0];
    auto replace_to = context.arguments[1];
    for (auto& str: context.text) {
        replace_all(str, replace_from, replace_to);
    }
}

void DumpWorker::run_operation(Context& context) {
    auto output = std::ofstream(context.arguments[0]);
    for (const auto& str: context.text) {
        output << str << std::endl;
    }
}


auto string_split(std::string str, std::string_view delimiter) {
    std::vector<std::string> result;
    std::size_t pos = 0;
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

Scheme WorkflowExecutor::parse(const TextContainer& text) const {
    const std::string delimiter = " = ";
    
    // First line show be desc
    if (text[0] != "desc") throw ParsingException("desc tag in first line expected", 0);

    Scheme scheme;
    auto i = 1;
    for (; text[i] != "csed"; i++) {
        auto pos = text[i].find(delimiter);
        if (pos == std::string::npos) throw ParsingException("delimiter not found", i);
        
        auto id = std::stoi(text[i].substr(0, pos));
        auto worker_with_arguments = text[i].substr(pos + delimiter.length(), text[i].length());
        
        auto arguments = static_cast<ArgumentList>(string_split(worker_with_arguments, " "));
        if (arguments.size() == 0) throw ParsingException("block name expected, but not found", i);
        
        auto worker_ptr = get_worker_by_name(arguments[0]);
        if (worker_ptr == nullptr) throw ParsingException("invalid worker name", i);

        arguments.erase(arguments.begin());

        scheme.id2worker.insert(std::make_pair(id, 
                                               std::make_pair(std::move(worker_ptr), std::move(arguments))
                                ));

        // csed should appear
        if (i + 1 == text.size()) throw ParsingException("csed expected, but not found", i);      
    }

    i++;

    if (i != text.size() - 1) throw ParsingException("id flow line expected, but not found", i); 

    auto flow_tokens = string_split(text[i], " -> ");

    for (const auto& s: flow_tokens) {
        scheme.execution_flow.push_back(std::stoi(s));
    }

    return scheme;
}

void WorkflowExecutor::validate(const Scheme &scheme) const {

}

Mode WorkflowExecutor::check_mode(const Scheme &scheme) const {
    return Mode::CmdlineFlagMode;
}

void WorkflowExecutor::execute(const Scheme &scheme, std::ifstream input, std::ofstream output) {

}

void WorkflowExecutor::execute(const Scheme &scheme) {

}
