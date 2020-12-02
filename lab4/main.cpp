#include <iostream>
#include <utility>
#include <tuple>
#include <string>
#include <fstream>

#include "tuple_print.h"
#include "CSVParser.h"

int main() {
    std::ifstream f("test.csv");
    CSVParser::CSVParser<std::string, int, float> parser(f, 0);
    std::cout << "Started reading" << std::endl;
    try {
        for (const auto& entry: parser) {
            std::cout << entry << std::endl;
        }
    } catch (CSVParser::ParsingException& e) {
        std::cout << e.what() << "; line: " << e.get_line_idx() << " char: " << e.get_char_idx() << std::endl;
    }
    std::cout << "Read all" << std::endl;
    return 0;
}
