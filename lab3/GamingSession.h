#pragma once

#include "Gamer.h"

enum class GamerType {
    RandomGamerType,
    StrategyGamerType,
    ConsoleGamerType
};

class GamingSession {
    int games_played = 0;
    int gamer_a_won = 0;
    int gamer_b_won = 0;

public:
    void play_game(InteractiveGameView& game_view, std::unique_ptr<UtilityGamer> a_gamer,
                   std::unique_ptr<UtilityGamer> b_gamer);
    void print_stats(GameView& game_view) const;
};