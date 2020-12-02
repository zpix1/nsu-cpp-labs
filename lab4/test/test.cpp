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
                                "noski, 10000, 3"};
        std::vector<std::tuple<std::string, int, float>> data{
                {"bipki", 10, 2.59},
                {"noski", 10000, 3.0}
        };

        CSVParser::CSVParser<std::string, int, float> parser(input, 1);
        int i = 0;
        for (const std::tuple<std::string, int, float>& rs: parser) {
            REQUIRE(rs == data[i]);
            i++;
        }
    }
    SECTION("correctly parses correct file with newline") {
        std::stringstream input{"title, count, price\n"
                                "bipki, 10, 2.59\n"
                                "noski, 10000, 3\n"};
        std::vector<std::tuple<std::string, int, float>> data{
                {"bipki", 10, 2.59},
                {"noski", 10000, 3.0}
        };

        CSVParser::CSVParser<std::string, int, float> parser(input, 1);
        int i = 0;
        for (const std::tuple<std::string, int, float>& rs: parser) {
            REQUIRE(rs == data[i]);
            i++;
        }
    }
    SECTION("raises if file has bad values") {
        std::stringstream input{"title, count, price\n"
                                "bipki, 10, 2.59\n"
                                "noski, bad, 3\n"};

        CSVParser::CSVParser<std::string, int, float> parser(input, 1);
        REQUIRE_THROWS_WITH([&]() {
            for (const std::tuple<std::string, int, float>& rs: parser);
        }(), Catch::Contains("incompatible type"));
    }

    SECTION("raises if file has  values with garbage") {
        std::stringstream input{"title, count, price\n"
                                "bipki, 10, 2.59 j\n"
                                "noski, bad, 3\n"};

        CSVParser::CSVParser<std::string, int, float> parser(input, 1);
        REQUIRE_THROWS_WITH([&]() {
            for (const std::tuple<std::string, int, float>& rs: parser);
        }(), Catch::Contains("garbage"));
    }

    SECTION("allows escaping") {
        std::stringstream input{"title, count, price\n"
                                "\"bipki, kepki, noski\", 10, 2.59\n"
                                "noski, 10000, 3"};
        std::vector<std::tuple<std::string, int, float>> data{
                {"bipki, kepki, noski", 10, 2.59},
                {"noski", 10000, 3.0}
        };

        CSVParser::CSVParser<std::string, int, float> parser(input, 1);
        int i = 0;
        for (const std::tuple<std::string, int, float>& rs: parser) {
            REQUIRE(rs == data[i]);
            i++;
        }
    }
}