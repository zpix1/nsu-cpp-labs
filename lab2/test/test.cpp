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

TEST_CASE("WorkflowExecutorTest", "[worker]") {
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
    WorkflowExecutor w;
    Scheme s = w.parse(instructions);
    REQUIRE(s.execution_flow == std::vector<int>{1, 2, 3, 5, 4});
    REQUIRE(s.id2worker[1].second == ArgumentList{"in.txt"});
    REQUIRE(s.id2worker[2].second == ArgumentList{"aaa"});
    REQUIRE(s.id2worker[3].second == ArgumentList{});
    REQUIRE(s.id2worker[4].second == ArgumentList{"out.txt"});
    REQUIRE(s.id2worker[5].second == ArgumentList{"abcd", "dcba"});
}