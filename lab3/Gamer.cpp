#include "Gamer.h"

#include <iostream>
#include <algorithm>
#include <random>

// Utility gamer stuff

MoveResult UtilityGamer::check_move(Move move) {
    int x = move.x;
    int y = move.y;
    if (my_field[x][y] == BattlefieldCellState::Ship) {
        my_field[x][y] = BattlefieldCellState::Hit;
        bool is_alive = is_ship_alive(my_field, x, y);
        if (!is_alive) {
            ships_count--;
            ship_destroy(my_field, x, y);
            return MoveResult::Destroyed;
        }
        return MoveResult::Hit;
    }
    if (my_field[x][y] == BattlefieldCellState::Hit) {
        return MoveResult::Hit;
    }
    if (my_field[x][y] == BattlefieldCellState::Destroyed) {
        return MoveResult::Destroyed;
    }
    return MoveResult::Missed;
}

bool UtilityGamer::lost() const {
    return ships_count == 0;
}

void UtilityGamer::init(GameView& game_view, const std::pair<int, Battlefield> ships_count_with_field) {
    ships_count = ships_count_with_field.first;
    my_field = ships_count_with_field.second;
}

// Random gamer stuff

std::pair<Move, MoveResult> RandomGamer::make_move(InteractiveGameView&, AnotherGamer& callback_gamer) {
    const Move move = {static_cast<int>(randint() % FIELD_HEIGHT), static_cast<int>(randint() % FIELD_WIDTH)};
    return std::pair<Move, MoveResult>{move, callback_gamer.check_move(move)};
}

// Console gamer stuff

void ConsoleGamer::prepare(InteractiveGameView& game_view) {
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        opponent_field.emplace_back(FIELD_WIDTH, BattlefieldCellState::Unknown);
    }
    auto [result, new_ships_count] = game_view.confirm_ship_placement(my_field);
    if (result) {
        ships_count = new_ships_count;
    }
}

std::pair<Move, MoveResult> ConsoleGamer::make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) {
    game_view.log("Your field:");
    game_view.render_field(my_field);

    game_view.log("Opponent's field:");
    game_view.render_field(opponent_field);

    auto move = game_view.ask_for_move();

    auto result = callback_gamer.check_move(move);

    if (result == MoveResult::Missed) {
        opponent_field[move.x][move.y] = BattlefieldCellState::Empty;
    } else if (result == MoveResult::Hit) {
        opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
    } else if (result == MoveResult::Destroyed) {
        opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
        ship_destroy(opponent_field, move.x, move.y, true);
    }

    return std::make_pair(move, result);
}

// Strategy gamer stuff

void StrategyGamer::prepare(InteractiveGameView&) {
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        opponent_field.emplace_back(FIELD_WIDTH, BattlefieldCellState::Unknown);
    }

    std::vector<int> other1;

    for (int field_id = 0; field_id < FIELD_WIDTH * FIELD_HEIGHT; field_id++) {
        int x = field_id / FIELD_HEIGHT;
        int y = field_id % FIELD_WIDTH;
        if ((x+y) % 2 == 0) {
            field_cell_order.push_back(field_id);
        } else if ((x+y) % 2 == 1) {
            other1.push_back(field_id);
        }
    }
    field_cell_order.insert(field_cell_order.end(), other1.begin(), other1.end());
}

std::pair<Move, MoveResult> StrategyGamer::make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) {
    Move move{};
    MoveResult result;
    if (state == State::Search) {
        bool exited = false;
        for (const int field_id : field_cell_order) {
            move = {field_id / FIELD_HEIGHT, field_id % FIELD_WIDTH};
            if (opponent_field[move.x][move.y] == BattlefieldCellState::Unknown) {
                result = callback_gamer.check_move(move);
                if (result == MoveResult::Missed) {
                    opponent_field[move.x][move.y] = BattlefieldCellState::Empty;
                } else if (result == MoveResult::Destroyed) {
                    opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
                    ship_destroy(opponent_field, move.x, move.y, true);
                } else if (result == MoveResult::Hit) {
                    opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
                    state = State::IdentifyDirection;
                    first_hit = move;
                    current_hit = move;
                }
                exited = true;
                break;
            }
        }
        if (!exited) {
            move = {0, 0};
            result = callback_gamer.check_move(move);
        }
    } else if (state == State::IdentifyDirection) {
        for (const auto& possible_direction: possible_directions) {
            move = {first_hit.x + possible_direction.x, first_hit.y + possible_direction.y};
            if (is_valid_point(move) && opponent_field[move.x][move.y] == BattlefieldCellState::Unknown) {
                result = callback_gamer.check_move(move);
                if (result == MoveResult::Hit) {
                    opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
                    current_direction = possible_direction;
                    current_hit = move;
                    state = State::FinishOff;
                } else if (result == MoveResult::Destroyed) {
                    opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
                    ship_destroy(opponent_field, move.x, move.y, true);
                    possible_directions = all_directions;
                    state = State::Search;
                } else if (result == MoveResult::Missed) {
                    opponent_field[move.x][move.y] = BattlefieldCellState::Empty;
                    possible_directions.erase(possible_direction);
                }
                break;
            }
        }
    } else if (state == State::FinishOff) {
        move = {current_hit.x + current_direction.x, current_hit.y + current_direction.y};
        if (is_valid_point(move)) {
            result = callback_gamer.check_move(move);
            if (result == MoveResult::Missed) {
                opponent_field[move.x][move.y] = BattlefieldCellState::Empty;
                current_hit = first_hit;
                current_direction = {current_direction.x * -1, current_direction.y * -1};
            } else if (result == MoveResult::Hit) {
                opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
                current_hit = move;
            } else if (result == MoveResult::Destroyed) {
                opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
                ship_destroy(opponent_field, move.x, move.y, true);
                possible_directions = all_directions;
                state = State::Search;
            }
        } else {
            current_hit = first_hit;
            current_direction = {current_direction.x * -1, current_direction.y * -1};
            return make_move(game_view, callback_gamer);
        }
    }
    return {move, result};
}
