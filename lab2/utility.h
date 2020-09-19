#ifndef LAB2_UTILITY_H
#define LAB2_UTILITY_H

#include "Worker.h"
#include <fstream>
#include <string>

#include <sys/stat.h>

namespace Workflow {
    // apparently some c++17 compilers don't have std::filesystem support
    inline void file_should_exist(const std::string& filename) {
        struct stat buffer;
        if (stat(filename.c_str(), &buffer) != 0) throw FileException("file does not exist", filename);
    }

    inline void readfile(const std::string& filename, TextContainer& text) {
        file_should_exist(filename);
        auto input = std::ifstream(filename);
        std::string str;
        while (std::getline(input, str)) {
            text.push_back(str);
        }
    }

    inline void readfile(std::istream& input, TextContainer& text) {
        std::string str;
        while (std::getline(input, str)) {
            text.push_back(str);
        }
    }

    inline void writefile(const std::string& filename, const TextContainer& text) {
        auto output = std::ofstream(filename);
        output.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        for (const auto& str: text) {
            output << str << std::endl;
        }
    }

    inline void writefile(std::ostream& output, const TextContainer& text) {
        for (const auto& str: text) {
            output << str << std::endl;
        }
    }
}

#endif //LAB2_UTILITY_H
