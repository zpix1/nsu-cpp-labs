//
// Created by John on 18.09.2020.
//

#ifndef LAB2_UTILITY_H
#define LAB2_UTILITY_H

#include "Worker.h"
#include <fstream>
#include <string>

#ifndef __has_include
static_assert(false, "__has_include not supported");
#else
#  if __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

namespace Workflow {

    inline void file_should_exist(const std::string& filename) {
        if (!fs::exists(filename)) throw FileException("file does not exist", filename);
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
