#include "Worker.h"

#include <fstream>
#include <iostream>
#include <algorithm>

void ReadfileWorker::run_operation(Context& context) {
    auto input = std::ifstream(context.arguments[0]);
    std::string str;
    while (std::getline(input, str)) {
        context.text.push_back(str);
    }
}

void WritefileWorker::run_operation(Context& context) {
    auto output = std::ofstream(context.arguments[0]);
    for (auto str: context.text) {
        output << str << std::endl;
    }
}

void GrepWorker::run_operation(Context& context) {
    auto grepstr = context.arguments[0];
    TextContainer new_text;
    for (auto str: context.text) {
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
void replace_all(std::string& str, std::string replace_from, std::string replace_to) {
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
    for (auto str: context.text) {
        output << str << std::endl;
    }
}
