#include <memory>
#include "GamingSession.h"
#include "thirdparty/CLI11.hpp"

std::unique_ptr<UtilityGamer> get_gamer_by_type(GamerType type) {
    if (type == GamerType::ConsoleGamerType) {
        return std::make_unique<ConsoleGamer>();
    }
    if (type == GamerType::StrategyGamerType) {
        return std::make_unique<StrategyGamer>();
    }
    if (type == GamerType::RandomGamerType) {
        return std::make_unique<RandomGamer>();
    }
    return std::make_unique<RandomGamer>();
}

int main(int argc, char **argv) {
    CLI::App app{"Battleship The Game"};

    int games_count = 1;
    app.add_option("-n", games_count, "Number of games to be played")->check(CLI::PositiveNumber);

    std::map<std::string, GamerType> name_to_type{
            {"random",   GamerType::RandomGamerType},
            {"console",  GamerType::ConsoleGamerType},
            {"strategy", GamerType::StrategyGamerType}
    };

    auto a_gamer_type = GamerType::ConsoleGamerType;
    app.add_option("-f, --first", a_gamer_type, "First gamer")->transform(
            CLI::CheckedTransformer(name_to_type, CLI::ignore_case));
    auto b_gamer_type = GamerType::StrategyGamerType;
    app.add_option("-s, --second", b_gamer_type, "Second gamer")->transform(
            CLI::CheckedTransformer(name_to_type, CLI::ignore_case));

    CLI11_PARSE(app, argc, argv)

    GamingSession s;
    ConsoleGameView gv;

    for (int i = 0; i < games_count; i++) {
        s.play_game(gv, get_gamer_by_type(a_gamer_type), get_gamer_by_type(b_gamer_type));
    }
    s.print_stats(gv);
    gv.pause();

    return 0;
}