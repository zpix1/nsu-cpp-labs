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
    std::cout << "Start reading" << std::endl;
    for (const auto& entry: parser) {
        std::cout << entry << std::endl;
    }
    std::cout << "Read all" << std::endl;
    return 0;
}
