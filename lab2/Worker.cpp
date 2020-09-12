//
// Created by John on 12.09.2020.
//

#include "Worker.h"

#include <fstream>

void ReadfileWorker::execute() {
    auto input = std::ifstream(argument);
    std::vector<std::string> text_values;
    std::string str;
    while (std::getline(input, str)) {
        text_values.push_back(str);
    }
    text = std::make_shared<std::vector<std::string>>(text_values);
}
