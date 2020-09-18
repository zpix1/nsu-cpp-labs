//
// Created by John on 18.09.2020.
//

#ifndef LAB2_UTILITY_H
#define LAB2_UTILITY_H

#include "Worker.h"
#include <fstream>
#include <string>

namespace Workflow {
    inline void readfile(const std::string &filename, TextContainer &text) {
        auto input = std::ifstream(filename);
        std::string str;
        while (std::getline(input, str)) {
            text.push_back(str);
        }
    }

    inline void readfile(std::istream& input, TextContainer &text) {
        std::string str;
        while (std::getline(input, str)) {
            text.push_back(str);
        }
    }

    inline void writefile(const std::string &filename, const TextContainer &text) {
        auto output = std::ofstream(filename);
        for (const auto &str: text) {
            output << str << std::endl;
        }
    }

    inline void writefile(std::ostream& output, const TextContainer &text) {
        for (const auto &str: text) {
            output << str << std::endl;
        }
    }
}

#endif //LAB2_UTILITY_H
