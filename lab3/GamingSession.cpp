#include <memory>

#include "GamingSession.h"

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

void GamingSession::play_game(InteractiveGameView& game_view, std::unique_ptr<UtilityGamer> a_gamer,
                              std::unique_ptr<UtilityGamer> b_gamer) {
//    std::unique_ptr<UtilityGamer> a_gamer = std::make_unique<ConsoleGamer>();
//    std::unique_ptr<UtilityGamer> b_gamer = std::make_unique<RandomGamer>();

    a_gamer->init(game_view, place_ships_randomly());
    b_gamer->init(game_view, place_ships_randomly());

    a_gamer->prepare(game_view);
    b_gamer->prepare(game_view);

    while (true) {
        // Gamer A move
        while (true) {
            auto[move, result] = a_gamer->make_move(game_view, *b_gamer);

            game_view.log("Gamer A selected cell " + std::to_string(move.x) + ":" + std::to_string(move.y));
            game_view.log("Result: " + move_result_to_string(result));

            if (b_gamer->lost()) {
                game_view.log("Gamer A won; Game ended;");
                game_view.log("Thanks for playing!");
                gamer_a_won++;
                games_played++;
                return;
            }
            if (result == MoveResult::Missed) {
                break;
            }
            game_view.log("Gamer A succeed, he can make another move");
        }

        game_view.pause();

        // Gamer B move
        while (true) {
            auto[move, result] = b_gamer->make_move(game_view, *a_gamer);

            game_view.log("Gamer B selected cell " + std::to_string(move.x) + ":" + std::to_string(move.y));
            game_view.log("Result: " + move_result_to_string(result));

            if (a_gamer->lost()) {
                game_view.log("Gamer B won; Game ended;");
                game_view.log("Thanks for playing!");
                gamer_b_won++;
                games_played++;
                return;
            }
            if (result == MoveResult::Missed) {
                break;
            }
            game_view.log("Gamer B succeed, he can make another move");
        }

        game_view.pause();
    }
}

void GamingSession::print_stats(GameView& game_view) const {
    game_view.log("Games played: " + std::to_string(games_played));
    game_view.log(
            "A won" + std::to_string(gamer_a_won) + " times, winrate: " + std::to_string(gamer_a_won * 1.0 / games_played));
    game_view.log(
            "B won" + std::to_string(gamer_b_won) + " times, winrate: " + std::to_string(gamer_b_won * 1.0 / games_played));
}
