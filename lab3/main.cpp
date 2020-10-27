#include "GameView.h"
#include "Gamer.h"

int main() {
    RandomGamer a_gamer;
    RandomGamer b_gamer;

    ConsoleGameView game_view;

    a_gamer.init(game_view);

    game_view.render_field(a_gamer.my_field);

//    b_gamer.init(game_view);

    return 0;

    while (true) {
        // Gamer A move
        auto [a_move, a_move_result] = a_gamer.make_move(b_gamer);

        game_view.log("Gamer A selected cell " + std::to_string(a_move.x) + ":" + std::to_string(a_move.y) );
        
        if (b_gamer.lost()) {
            game_view.log("Gamer A won; Game ended;");
            break;
        }

        // Gamer B move
        auto [b_move, b_move_result] = b_gamer.make_move(a_gamer);

        game_view.log("Gamer A selected cell " + std::to_string(b_move.x) + ":" + std::to_string(b_move.y) );

        if (a_gamer.lost()) {
            game_view.log("Gamer B won; Game ended;");
            break;
        }
    }

    game_view.log("Thanks for playing!");

    return 0;
}