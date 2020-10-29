#include <memory>
#include "GamingSession.h"
#include "thirdparty/CLI11.hpp"

std::unique_ptr<UtilityGamer> get_gamer_by_str(const std::string& type) {
    if (type == "console") {
        return std::make_unique<ConsoleGamer>();
    }
    if (type == "strategy") {
        return std::make_unique<StrategyGamer>();
    }
    if (type == "random") {
        return std::make_unique<RandomGamer>();
    }
    return std::make_unique<RandomGamer>();
}

int main(int argc, char** argv) {
    CLI::App app{"Battleship The Game", "battleship"};

    int games_count = 1;
    app.add_option("-n", games_count, "Number of games to be played");

    std::string a_name = "console";
    app.add_set("-f, --first", a_name, {"console", "random", "strategy"}, "First gamer");
    std::string b_name = "strategy";
    app.add_set("-s, --second", b_name, {"console", "random", "strategy"}, "Second gamer");

    CLI11_PARSE(app, argc, argv);

    GamingSession s;
    ConsoleGameView gv;

    for (int i = 0; i < games_count; i++) {
        s.play_game(gv, get_gamer_by_str(a_name), get_gamer_by_str(b_name));
    }
    s.print_stats(gv);
    gv.pause();

    return 0;
}