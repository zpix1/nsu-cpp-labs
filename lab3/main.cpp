#include <memory>
#include "GamingSession.h"
#include "GameView.h"

int main() {
    GamingSession s;
    ConsoleGameView gv;
    s.play_game(gv, std::make_unique<ConsoleGamer>(), std::make_unique<StrategyGamer>());
    s.print_stats(gv);
    gv.pause();
    return 0;
}