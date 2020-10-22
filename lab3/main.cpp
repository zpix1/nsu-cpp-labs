#include "GameView.h"
#include "Gamer.h"

int main() {
    ConsoleGamer a_gamer;
    RandomGamer b_gamer;

    ConsoleGameView gameview;

    a_gamer.init(gameview);
    b_gamer.init(gameview);

    while (true) {
        // Gamer A move
        auto [a_move, a_move_result] = a_gamer.make_move(b_gamer);
        
        if (b_gamer.lost()) {
            gameview.log("Gamer A won; Game ended;");
            break;
        }

        // Gamer B move
        auto [b_move, b_move_result] = b_gamer.make_move(a_gamer);

        if (a_gamer.lost()) {
            gameview.log("Gamer B won; Game ended;");
            break;
        }
    }

    gameview.log("Thanks for playing!");

    return 0;
}