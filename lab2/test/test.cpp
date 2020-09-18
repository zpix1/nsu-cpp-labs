#define CATCH_CONFIG_MAIN

#include "../thirdparty/catch.hpp"

#include "../Worker.h"
#include "../utility.h"
#include <sstream>

using namespace Workflow;

const char *TEMP_FILENAME{"temp.txt"};

TEST_CASE("GrepWorkerTest", "[worker]") {
    Context c{
            {"123ab", "321", "abcd"}
    };
    GrepWorker w;
    w.run_operation({"ab"}, c);
    REQUIRE(c.text == TextContainer{
            "123ab",
            "abcd"
    });
}

TEST_CASE("SortWorkerTest", "[worker]") {
    Context c{
            {"bcda", "abcd", "zzzz"}
    };
    SortWorker w;
    w.run_operation({}, c);
    REQUIRE(c.text == TextContainer{
            "abcd",
            "bcda",
            "zzzz"
    });
}

TEST_CASE("ReplaceWorkerTest", "[worker]") {
    SECTION("short to long") {
        Context c{
                {"bcdabkekab", "abcd", "zzzz"}
        };
        ReplaceWorker w;
        w.run_operation({"ab", "<replaced>"}, c);
        REQUIRE(c.text == TextContainer{
                "bcd<replaced>kek<replaced>",
                "<replaced>cd",
                "zzzz"
        });
    }SECTION("long to short") {
        Context c{
                {"abcdefabcdef", "aabcdefa", "zzzz"}
        };
        ReplaceWorker w;
        w.run_operation({"abcdef", "<>"}, c);
        REQUIRE(c.text == TextContainer{
                "<><>",
                "a<>a",
                "zzzz"
        });
    }
}

TEST_CASE("DumpWorkerTest", "[worker]") {
    Context c{
            {"abcdefabcdef", "aabcdefa", "zzzz"}
    };
    DumpWorker w;
    w.run_operation({TEMP_FILENAME}, c);
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
    SECTION("valid instructions FlagZero") {
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
        }SECTION("validates correctly") {
            REQUIRE_NOTHROW(worker.validate(scheme, InputOutputMode::FlagZero));
        }SECTION("runs correctly") {
            std::ofstream output("in.txt");
            writefile(output, TextContainer{
                    "jejeje lololo",
                    "z jeaaaaaaajeje",
                    "a jejeje lololoaaa",
                    "y aaa abcd"
            });
            worker.execute(scheme);
            TextContainer out;
            readfile("out.txt", out);
            REQUIRE(out == TextContainer{
                    "a jejeje lololoaaa",
                    "y aaa dcba",
                    "z jeaaaaaaajeje"
            });
        }
    }SECTION("valid instructions FlagIO") {
        TextContainer instructions{
                "desc",
                "2 = grep aaa",
                "3 = sort",
                "5 = replace abcd dcba",
                "csed",
                "2 -> 3 -> 5"
        };

        WorkflowExecutor worker;
        Scheme scheme;
        scheme = worker.parse(instructions);
        SECTION("parses correctly") {
            REQUIRE(scheme.execution_flow == std::vector<WorkerID>{2, 3, 5});
            REQUIRE(scheme.id2worker[2].second == ArgumentList{"aaa"});
            REQUIRE(scheme.id2worker[3].second.empty());
            REQUIRE(scheme.id2worker[5].second == ArgumentList{"abcd", "dcba"});
        }SECTION("validates correctly") {
            REQUIRE_NOTHROW(worker.validate(scheme, InputOutputMode::FlagIO));
        }SECTION("runs correctly") {
            std::string input_str{
                    "jejeje lololo\n"
                    "z jeaaaaaaajeje\n"
                    "a jejeje lololoaaa\n"
                    "y aaa abcd\n"
            };
            std::stringstream input{input_str};
            std::stringstream output;
            worker.execute(scheme, input, output);

            TextContainer out;
            readfile(output, out);
            REQUIRE(out == TextContainer{
                    "a jejeje lololoaaa",
                    "y aaa dcba",
                    "z jeaaaaaaajeje"
            });
        }
    }SECTION("empty flow instructions") {
        TextContainer instructions{
                "desc",
                "2 = grep aaa",
                "3 = sort",
                "5 = replace abcd dcba",
                "csed",
                ""
        };

        WorkflowExecutor worker;
        Scheme scheme;
        scheme = worker.parse(instructions);
        SECTION("parses correctly") {
            REQUIRE(scheme.execution_flow.empty());
            REQUIRE(scheme.id2worker[2].second == ArgumentList{"aaa"});
            REQUIRE(scheme.id2worker[3].second.empty());
            REQUIRE(scheme.id2worker[5].second == ArgumentList{"abcd", "dcba"});
        }SECTION("validates correctly") {
            REQUIRE_NOTHROW(worker.validate(scheme, InputOutputMode::FlagIO));
        }SECTION("runs correctly") {
            std::string input_str{
                    "jejeje lololo\n"
                    "z jeaaaaaaajeje\n"
                    "a jejeje lololoaaa\n"
                    "y aaa abcd\n"
            };
            std::stringstream input{input_str};
            std::stringstream output;
            worker.execute(scheme, input, output);

            TextContainer out;
            readfile(output, out);
            REQUIRE(out == TextContainer{
                    "jejeje lololo",
                    "z jeaaaaaaajeje",
                    "a jejeje lololoaaa",
                    "y aaa abcd"
            });
        }
    }SECTION("worker integration") {
        std::vector<std::tuple<std::string, TextContainer, std::string, TextContainer>> tests{
                {"show ERROR log entries",                                      {"desc", "1 = grep :ERROR:",  "csed",           "1"},              {"01.01.2020:WARNING:Juju\n01.02.2020:ERROR:Very bad error\n01.03.2020:ERROR:Very very bad error"}, {"01.02.2020:ERROR:Very bad error", "01.03.2020:ERROR:Very very bad error"}},
                {"replace all digits to A and select lines with more than 3 A", {"desc", "1 = replace \\d A", "2 = grep A{3,}", "csed", "1 -> 2"}, {"kek\nls12\nms123\ndls123456"},                                                                    {"msAAA",                           "dlsAAAAAA"}},
        };
        for (const auto&[name, instructions, input_str, output_str] : tests) {
            SECTION(name) {
                WorkflowExecutor worker;
                auto scheme = worker.parse(instructions);
                REQUIRE_NOTHROW(worker.validate(scheme, InputOutputMode::FlagIO));

                std::stringstream input{input_str};
                std::stringstream output;
                worker.execute(scheme, input, output);

                TextContainer out;
                readfile(output, out);
                REQUIRE(out == output_str);
            }
        }
    }SECTION("invalid instructions parsing exceptions") {
        WorkflowExecutor worker;
        std::vector<std::tuple<TextContainer, std::string, InputOutputMode> > tests{
                {{"1 = readfile a.txt", "2 = sort",                                 "3 = writefile b.txt", "csed",                   "1 -> 2 -> 3"},                                       "desc",            InputOutputMode::FlagZero},
                {{"desc",               "1 = readfile a.txt",                       "2 = sort",            "3 = writefile b.txt",    "1 -> 2 -> 3"},                                       "delimiter",       InputOutputMode::FlagZero},
                {{"desc",               "1 = readfile a.txt",                       "2 sort",              "3 = writefile b.txt",    "csed",                "1 -> 2 -> 3"},                "delimiter",       InputOutputMode::FlagZero},
                {{"desc",               "1 = readfile a.txt",                       "2 = sort",            "3 = wwwwritefile b.txt", "csed",                "1 -> 2 -> 3"},                "worker name",     InputOutputMode::FlagZero},
                {{"desc",               "1 = readfile a.txt",                       "2 = sort",            "2 = dump",               "3 = writefile b.txt", "csed",        "1 -> 2 -> 3"}, "repetition",      InputOutputMode::FlagZero},
                {{"desc",               "1 = readfile a.txt",                       "2 = sort",            "3 = writefile b.txt",    "csed",                "1 -> 2 -> 3", "trash"},       "flow line",       InputOutputMode::FlagZero},
                {{"desc",               "1 = readfile a.txt",                       "2 = sort",            "3 = writefile b.txt",    "csed",                "1 -> 2 -> 3 -> 100"},         "undeclared id",   InputOutputMode::FlagZero},
                {{"desc",               "kwkw = readfile a.txt",                    "2 = sort",            "3 = writefile b.txt",    "csed",                "1 -> 2 -> 3"},                "invalid id",      InputOutputMode::FlagZero},
                {{"desc",               "12342134123412677676345 = readfile a.txt", "2 = sort",            "3 = writefile b.txt",    "csed",                "1 -> 2 -> 3"},                "out-of-range id", InputOutputMode::FlagZero},
                {{"desc",               "-1 = readfile a.txt",                      "2 = sort",            "3 = writefile b.txt",    "csed",                "1 -> 2 -> 3"},                "negative id",     InputOutputMode::FlagZero},
        };
        for (const auto&[instructions, exception_text, mode] : tests) {
            SECTION("exception " + exception_text) {
                REQUIRE_THROWS_WITH(worker.parse(instructions), Catch::Contains(exception_text));
            }
        }
    }SECTION("invalid instructions validating exceptions") {
        WorkflowExecutor worker;
        std::vector<std::tuple<TextContainer, std::string, InputOutputMode> > tests{
                {{"desc", "1 = readfile a.txt b.txt", "2 = sort", "3 = writefile b.txt", "csed", "1 -> 2 -> 3"}, "invalid arguments",                               InputOutputMode::FlagZero},
                {{"desc", "1 = readfile a.txt",       "2 = sort", "3 = writefile b.txt", "csed", ""},            "not enough flow elements",                        InputOutputMode::FlagI},
                {{"desc", "1 = readfile a.txt",       "2 = sort", "3 = writefile b.txt", "csed", "1"},           "not enough flow elements",                        InputOutputMode::FlagZero},
                {{"desc", "1 = readfile a.txt",       "2 = sort", "3 = writefile b.txt", "csed", "2 -> 3"},      "read file worker",                                InputOutputMode::FlagZero},
                {{"desc", "1 = readfile a.txt",       "2 = sort", "3 = writefile b.txt", "csed", "1 -> 2"},      "write file worker",                               InputOutputMode::FlagZero},
                {{"desc", "1 = readfile a.txt",       "2 = sort", "3 = writefile b.txt", "csed", "1 -> 2 -> 3"}, "read or write worker unexpected in command line", InputOutputMode::FlagI},
                {{"desc", "1 = readfile a.txt",       "2 = sort", "3 = writefile b.txt", "csed", "1 -> 2 -> 3"}, "read or write worker unexpected in command",      InputOutputMode::FlagO},
                {{"desc", "1 = readfile a.txt",       "2 = sort", "3 = writefile b.txt", "csed", "1 -> 2 -> 3"}, "read or write worker unexpected in",              InputOutputMode::FlagIO},
        };
        for (const auto&[instructions, exception_text, mode] : tests) {
            SECTION("exception " + exception_text) {
                Scheme s = worker.parse(instructions);
                REQUIRE_THROWS_WITH(worker.validate(s, mode), Catch::Contains(exception_text));
            }
        }
    }
}