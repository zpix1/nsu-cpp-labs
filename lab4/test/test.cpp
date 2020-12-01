#define CATCH_CONFIG_MAIN

#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "../thirdparty/catch.hpp"

#include "../CSVParser.h"

TEST_CASE("stream test") {
    SECTION("correctly parses correct file") {
        std::stringstream input{"title, count, price\n"
                                "bipki, 10, 2.59\n"
                                "noski, 10000, 3\n"};
        std::vector<std::tuple<std::string, int, float>> data{
                {"bipki", 10, 2.59},
                {"noski", 10000, 3.0}
        };

        CSVParser<std::string, int, float> parser(input, 1);
        int i = 0;
        for (std::tuple<std::string, int, float> rs: parser) {
            REQUIRE(rs == data[i]);
            i++;
        }
    }
}