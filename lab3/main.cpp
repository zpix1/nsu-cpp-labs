#include "GamingSession.h"
#include "GameView.h"

int main() {
    GamingSession s;
    ConsoleGameView gv;
    s.play_game(gv, GamerType::RandomGamerType, GamerType::RandomGamerType);
    s.print_stats(gv);
    gv.pause();
    return 0;
}