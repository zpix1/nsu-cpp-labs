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
    REQUIRE(c.text.size() == 2);
    REQUIRE(c.text[0] == "123ab");
    REQUIRE(c.text[1] == "abcd");
}

TEST_CASE("SortWorkerTest", "[worker]") {
    Context c{
        { },
        {"bcda", "abcd", "zzzz"}
    };
    SortWorker w;
    w.run_operation(c);
    REQUIRE(c.text.size() == 3);
    REQUIRE(c.text[0] == "abcd");
    REQUIRE(c.text[1] == "bcda");
    REQUIRE(c.text[2] == "zzzz");
}

TEST_CASE("ReplaceWorkerTest", "[worker]") {
    SECTION("short to long") {
        Context c{
            { "ab", "<replaced>" },
            { "bcdabkekab", "abcd", "zzzz" }
        };
        ReplaceWorker w;
        w.run_operation(c);
        REQUIRE(c.text.size() == 3);
        REQUIRE(c.text[0] == "bcd<replaced>kek<replaced>");
        REQUIRE(c.text[1] == "<replaced>cd");
        REQUIRE(c.text[2] == "zzzz");
    }
    SECTION("long to short") {
        Context c{
            { "abcdef", "<>" },
            { "abcdefabcdef", "aabcdefa", "zzzz" }
        };
        ReplaceWorker w;
        w.run_operation(c);
        REQUIRE(c.text.size() == 3);
        REQUIRE(c.text[0] == "<><>");
        REQUIRE(c.text[1] == "a<>a");
        REQUIRE(c.text[2] == "zzzz");
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
        "csed",
        "1 -> 2 -> 3 -> 4"
    };
    WorkflowExecutor w;
    Scheme s = w.parse(instructions);
}

TEST_CASE("Failed test", "[worker]") {
    REQUIRE(2+2 == 5);
}