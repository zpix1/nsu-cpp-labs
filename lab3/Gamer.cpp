#include "Gamer.h"

#include <iostream>
#include <algorithm>
#include <random>

static bool ship_dfs_is_alive(std::vector<std::vector<bool>>& used, const Battlefield& field, int x, int y) {
    if (used[x][y])
        return false;
    used[x][y] = true;

    if (field[x][y] == BattlefieldCellState::Ship)
        return true;
    if (field[x][y] != BattlefieldCellState::Hit)
        return false;
    bool is_alive = false;
    for (const auto[x_neighbour, y_neighbour]: NEIGHBOURS) {
        int x_check = x + x_neighbour;
        int y_check = y + y_neighbour;
        if (is_valid_point(x_check, y_check)) {
            is_alive = is_alive || ship_dfs_is_alive(used, field, x_check, y_check);
        }
    }
    return is_alive;
}

// Ship is alive if only at least one of its cells is alive
static bool is_ship_alive(const Battlefield& field, int x, int y) {
    std::vector<std::vector<bool>> used(FIELD_HEIGHT, std::vector<bool>(FIELD_WIDTH));
    return ship_dfs_is_alive(used, field, x, y);
}

static void ship_dfs_destroy(std::vector<std::vector<bool>>& used, Battlefield& field, int x, int y, bool mark_locked) {
    if (used[x][y])
        return;
    used[x][y] = true;
    if (field[x][y] == BattlefieldCellState::Unknown) {
        field[x][y] = BattlefieldCellState::Locked;
        return;
    }
    if (field[x][y] != BattlefieldCellState::Hit)
        return;
    field[x][y] = BattlefieldCellState::Destroyed;
    for (const auto[x_neighbour, y_neighbour]: NEIGHBOURS) {
        int x_check = x + x_neighbour;
        int y_check = y + y_neighbour;
        if (is_valid_point(x_check, y_check)) {
            ship_dfs_destroy(used, field, x_check, y_check, mark_locked);
        }
    }
}

// Ship is alive if only at least one of its cells is alive
static void ship_destroy(Battlefield& field, int x, int y, bool mark_locked = false) {
    std::vector<std::vector<bool>> used(FIELD_HEIGHT, std::vector<bool>(FIELD_WIDTH));
    ship_dfs_destroy(used, field, x, y, mark_locked);
}

std::pair<int, Battlefield> place_ships_randomly() {
    Battlefield my_field;
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        my_field.emplace_back(FIELD_WIDTH, BattlefieldCellState::Empty);
    }

    int ships_placed = 0;
    for (const auto& base_ship: SHIPS) {
        std::vector<Ship> ship_rotations{base_ship};
        if (base_ship.x != base_ship.y) {
            ship_rotations.push_back({base_ship.y, base_ship.x});
        }
        std::shuffle(ship_rotations.begin(), ship_rotations.end(), std::mt19937(std::random_device()()));

        for (const auto& ship: ship_rotations) {
            for (int try_n = 0; try_n < 100; try_n++) {
                int x = randint() % FIELD_HEIGHT;
                int y = randint() % FIELD_WIDTH;
                for (const auto[direction_x, direction_y]: DIRECTIONS) {
                    bool can_place = true;
                    for (int x_part_pos = x;
                         x_part_pos != x + direction_x * ship.x; x_part_pos += direction_x) {
                        for (int y_part_pos = y;
                             y_part_pos != y + direction_y * ship.y; y_part_pos += direction_y) {
                            if (is_valid_point(x_part_pos, y_part_pos) &&
                                my_field[x_part_pos][y_part_pos] == BattlefieldCellState::Empty) {
                                for (const auto[x_neighbour, y_neighbour]: NEIGHBOURS) {
                                    int x_check = x_part_pos + x_neighbour;
                                    int y_check = y_part_pos + y_neighbour;

                                    if (is_valid_point(x_check, y_check)) {
                                        if (my_field[x_check][y_check] != BattlefieldCellState::Empty) {
                                            can_place = false;
                                            goto RESULT_FOUND;
                                        }
                                    }
                                }
                            } else {
                                can_place = false;
                                goto RESULT_FOUND;
                            }
                        }
                    }
                    RESULT_FOUND:;
                    if (can_place) {
                        for (int x_part_pos = x;
                             x_part_pos != x + direction_x * (ship.x); x_part_pos += direction_x) {
                            for (int y_part_pos = y;
                                 y_part_pos != y + direction_y * (ship.y); y_part_pos += direction_y) {
                                my_field[x_part_pos][y_part_pos] = BattlefieldCellState::Ship;
                            }
                        }
                        ships_placed++;
                        goto NEW_SHIP;
                    }
                }
            }
        }
        NEW_SHIP:;
    }
    return std::make_pair(ships_placed, my_field);
}

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
    ships_count = game_view.confirm_ship_placement(my_field);
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

    std::vector<int> other;
    for (int field_id = 0; field_id < FIELD_WIDTH * FIELD_HEIGHT; field_id++) {
        int x = field_id / FIELD_HEIGHT;
        int y = field_id % FIELD_WIDTH;
        if ((x + y) % 2 == 0) {
            field_cell_order.push_back(field_id);
        } else {
            other.push_back(field_id);
        }
    }
    field_cell_order.insert(field_cell_order.end(), other.begin(), other.end());
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
