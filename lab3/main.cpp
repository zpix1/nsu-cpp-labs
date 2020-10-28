#include "GameView.h"
#include "Gamer.h"

std::string move_result_to_string(MoveResult result) {
    if (result == MoveResult::Missed) {
        return "Missed";
    }
    if (result == MoveResult::Hit) {
        return "Hit";
    }
    if (result == MoveResult::Destroyed) {
        return "Destroyed";
    }
    return "???";
}

int play_game() {
    RandomGamer a_gamer;
    StrategyGamer b_gamer;

    ConsoleGameView game_view;

    a_gamer.init(game_view);
    b_gamer.init(game_view);

    while (true) {
        // Gamer A move
        while (true) {
            auto[a_move, a_move_result] = a_gamer.make_move(game_view, b_gamer);

//            game_view.log("Gamer A selected cell " + std::to_string(a_move.x) + ":" + std::to_string(a_move.y));
//            game_view.log("Result: " + move_result_to_string(a_move_result));

            if (b_gamer.lost()) {
                game_view.log("Gamer A won; Game ended;");
                game_view.log("Thanks for playing!");
                return 0;
            }
            if (a_move_result == MoveResult::Missed) {
                break;
            }
            game_view.log("Gamer A succeed, he can make another move");
        }

        game_view.pause();

        // Gamer B move
        while (true) {
            auto[b_move, b_move_result] = b_gamer.make_move(game_view, a_gamer);

            game_view.log("Gamer B selected cell " + std::to_string(b_move.x) + ":" + std::to_string(b_move.y));
            game_view.log("Result: " + move_result_to_string(b_move_result));
            game_view.render_field(b_gamer.opponent_field);
            if (a_gamer.lost()) {
                game_view.log("Gamer B won; Game ended;");
                game_view.log("Thanks for playing!");
                return 1;
            }
            if (b_move_result == MoveResult::Missed) {
                break;
            }
            game_view.log("Gamer B succeed, he can make another move");
        }

        game_view.pause();
    }
}

int main() {
    play_game();
    return 0;
}