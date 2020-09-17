#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../Worker.h"

using namespace Workflow;

const std::string TEMP_FILENAME{ "temp.txt" };

TEST_CASE("GrepWorkerTest", "[worker]") {
    Context c{
        { "ab" },
        {"123ab", "321", "abcd"}
    };
    GrepWorker w;
    w.run_operation(c);
    REQUIRE(c.text == TextContainer{
        "123ab",
        "abcd"
    });
}

TEST_CASE("SortWorkerTest", "[worker]") {
    Context c{
        { },
        {"bcda", "abcd", "zzzz"}
    };
    SortWorker w;
    w.run_operation(c);
    REQUIRE(c.text == TextContainer{
        "abcd",
        "bcda",
        "zzzz"
    });
}

TEST_CASE("ReplaceWorkerTest", "[worker]") {
    SECTION("short to long") {
        Context c{
            { "ab", "<replaced>" },
            { "bcdabkekab", "abcd", "zzzz" }
        };
        ReplaceWorker w;
        w.run_operation(c);
        REQUIRE(c.text == TextContainer{
            "bcd<replaced>kek<replaced>",
            "<replaced>cd",
            "zzzz"
        });
    }
    SECTION("long to short") {
        Context c{
            { "abcdef", "<>" },
            { "abcdefabcdef", "aabcdefa", "zzzz" }
        };
        ReplaceWorker w;
        w.run_operation(c);
        REQUIRE(c.text == TextContainer{
            "<><>",
            "a<>a",
            "zzzz"
        });
    }
}

TEST_CASE("DumpWorkerTest", "[worker]") {
    Context c{
        { TEMP_FILENAME },
        { "abcdefabcdef", "aabcdefa", "zzzz" }
    };
    DumpWorker w;
    w.run_operation(c);
    REQUIRE(c.text.size() == 3);
    std::ifstream f(TEMP_FILENAME);
    std::string s;
    std::getline(f, s);
    REQUIRE(s == "abcdefabcdef");
    std::getline(f, s);
    REQUIRE(s == "aabcdefa");
    std::getline(f, s);
    REQUIRE(s == "zzzz");
}

TextContainer readfile(const std::string& fname) {
    TextContainer t;
    auto input = std::ifstream(fname);
    std::string str;
    while (std::getline(input, str)) {
        t.push_back(str);
    }
    return t;
}

void writefile(const std::string& fname, const TextContainer& text) {
    auto output = std::ofstream(fname);
    for (const auto &str: text) {
        output << str << std::endl;
    }
}

TEST_CASE("WorkflowExecutorTest", "[worker]") {
    SECTION("correct instructions") {
        TextContainer instructions{
                "desc",
                "1 = readfile in.txt",
                "2 = grep aaa",
                "3 = sort",
                "4 = writefile out.txt",
                "5 = replace abcd dcba",
                "csed",
                "1 -> 2 -> 3 -> 5 -> 4"
        };

        WorkflowExecutor worker;
        Scheme scheme;
        scheme = worker.parse(instructions);
        SECTION("parses correctly") {
            REQUIRE(scheme.execution_flow == std::vector<WorkerID>{1, 2, 3, 5, 4});
            REQUIRE(scheme.id2worker[1].second == ArgumentList{"in.txt"});
            REQUIRE(scheme.id2worker[2].second == ArgumentList{"aaa"});
            REQUIRE(scheme.id2worker[3].second.empty());
            REQUIRE(scheme.id2worker[4].second == ArgumentList{"out.txt"});
            REQUIRE(scheme.id2worker[5].second == ArgumentList{"abcd", "dcba"});
        }
        SECTION("validates correctly") {
            auto validation_result = worker.validate(scheme, InputOutputMode::FlagZero);
            REQUIRE(validation_result == true);
        }
        SECTION("runs correctly") {
            writefile("in.txt", TextContainer{
                "jejeje lololo",
                "z jeaaaaaaajeje",
                "a jejeje lololoaaa",
                "y aaa abcd"
            });
            worker.execute(scheme);
            auto out = readfile("out.txt");
            REQUIRE(out == TextContainer{
                "a jejeje lololoaaa",
                "y aaa dcba",
                "z jeaaaaaaajeje"
            });
        }
    }
}