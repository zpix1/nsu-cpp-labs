#include "Gamer.h"

#include <iostream>
#include <algorithm>
#include <random>


bool ship_dfs_is_alive(std::vector<std::vector<bool>>& used, const Battlefield& field, int x, int y) {
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
bool is_ship_alive(const Battlefield& field, int x, int y) {
    std::vector<std::vector<bool>> used(FIELD_HEIGHT, std::vector<bool>(FIELD_WIDTH));
    return ship_dfs_is_alive(used, field, x, y);
}

void ship_dfs_destroy(std::vector<std::vector<bool>>& used, Battlefield& field, int x, int y, bool markn) {
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
            ship_dfs_destroy(used, field, x_check, y_check, markn);
        }
    }
}
// Ship is alive if only at least one of its cells is alive
void ship_destroy(Battlefield& field, int x, int y, bool markn = false) {
    std::vector<std::vector<bool>> used(FIELD_HEIGHT, std::vector<bool>(FIELD_WIDTH));
    ship_dfs_destroy(used, field, x, y, markn);
}

void place_ships_randomly(Battlefield& my_field) {
    for (const auto& base_ship: SHIPS) {
        std::vector<Ship> ship_rotations{base_ship};
        if (base_ship.width != base_ship.height) {
            ship_rotations.push_back({base_ship.height, base_ship.width});
        }
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(ship_rotations.begin(), ship_rotations.end(), std::mt19937(std::random_device()()));

        for (const auto& ship: ship_rotations) {
            for (int tryno = 0; tryno < 100; tryno++) {
                int x = rand() % FIELD_HEIGHT;
                int y = rand() % FIELD_WIDTH;
                for (const auto[direction_x, direction_y]: DIRECTIONS) {
                    bool can_place = true;
                    for (int x_part_pos = x;
                         x_part_pos != x + direction_x * ship.width; x_part_pos += direction_x) {
                        for (int y_part_pos = y;
                             y_part_pos != y + direction_y * ship.height; y_part_pos += direction_y) {
                            if (is_valid_point(x_part_pos, y_part_pos) &&
                                my_field[x_part_pos][y_part_pos] == BattlefieldCellState::Empty) {
                                for (const auto[x_neighbour, y_neighbour]: NEIGHBOURS) {
                                    int x_check = x_part_pos + x_neighbour;
                                    int y_check = y_part_pos + y_neighbour;

                                    if (is_valid_point(x_check, y_check)) {
                                        if (my_field[x_check][y_check] != BattlefieldCellState::Empty) {
                                            can_place = false;
                                            goto XYPARTEND;
                                        }
                                    }
                                }
                            } else {
                                can_place = false;
                                goto XYPARTEND;
                            }
                        }
                    }
                    XYPARTEND:;
                    if (can_place) {
                        for (int x_part_pos = x;
                             x_part_pos != x + direction_x * (ship.width); x_part_pos += direction_x) {
                            for (int y_part_pos = y;
                                 y_part_pos != y + direction_y * (ship.height); y_part_pos += direction_y) {
                                my_field[x_part_pos][y_part_pos] = BattlefieldCellState::Ship;
                            }
                        }
//                        std::cout << "Ship placed: " << x << ":"<< y << " with size " << ship.width << ":" << ship.height << std::endl;
                        goto NEWSHIP;
                    }
                }
            }
        }
        NEWSHIP:;
    }
}

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

// Random gamer stuff

void RandomGamer::init(GameView&) {
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        my_field.emplace_back(FIELD_WIDTH, BattlefieldCellState::Empty);
    }

    place_ships_randomly(my_field);
}

std::pair<Move, MoveResult> RandomGamer::make_move(InteractiveGameView&, AnotherGamer& callback_gamer) {
    const Move move = {rand() % FIELD_HEIGHT, rand() % FIELD_WIDTH};
    return std::pair<Move, MoveResult>{move, callback_gamer.check_move(move)};
}

// Console gamer stuff

void ConsoleGamer::init(GameView& game_view) {
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        my_field.emplace_back(FIELD_WIDTH, BattlefieldCellState::Empty);
        opponent_field.emplace_back(FIELD_WIDTH, BattlefieldCellState::Unknown);
    }

    place_ships_randomly(my_field);
}

std::pair<Move, MoveResult> ConsoleGamer::make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) {
    game_view.log("Your field:");
    game_view.render_field(my_field);

    game_view.log("Opponent's field:");
    game_view.render_field(opponent_field);

    auto move = game_view.ask_for_move("");

    auto result = callback_gamer.check_move(move);

    if (result == MoveResult::Missed) {
        opponent_field[move.x][move.y] = BattlefieldCellState::Empty;
    } else if (result == MoveResult::Hit) {
        opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
    } else if (result == MoveResult::Destroyed) {
        opponent_field[move.x][move.y] = BattlefieldCellState::Hit;
        ship_destroy(opponent_field, move.x, move.y, true);
    }

    game_view.clear();

    return std::pair<Move, MoveResult>{move, result};
}

// Interactive gamer stuff

void StrategyGamer::init(GameView& game_view) {
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        my_field.emplace_back(FIELD_WIDTH, BattlefieldCellState::Empty);
        opponent_field.emplace_back(FIELD_WIDTH, BattlefieldCellState::Unknown);
    }

    place_ships_randomly(my_field);
}

std::pair<Move, MoveResult> StrategyGamer::make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) {
    const Move move = {rand() % FIELD_HEIGHT, rand() % FIELD_WIDTH};

    return std::pair<Move, MoveResult>();
}
