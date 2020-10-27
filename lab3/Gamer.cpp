#include "Gamer.h"

#include <iostream>

// Random gamer

bool is_valid_point(int x, int y) {
    if (x >= 0 && x < FIELD_WIDTH)
        if (y >= 0 && y < FIELD_HEIGHT)
            return true;
    return false;
}

void RandomGamer::init(GameView&) {
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        my_field.emplace_back(FIELD_WIDTH, BattlefieldCellState::Empty);
    }

    for (const auto& base_ship: SHIPS) {
        std::vector<Ship> ship_rotations{base_ship};
        if (base_ship.width != base_ship.height) {
            ship_rotations.push_back({base_ship.height, base_ship.width});
        }
        for (const auto& ship: ship_rotations) {
            for (int tryno = 0; tryno < 100; tryno++) {
                int x = rand() % FIELD_WIDTH;
                int y = rand() % FIELD_HEIGHT;
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

MoveResult RandomGamer::check_move(Move move) {
    return MoveResult::Destroyed;
}

std::pair<Move, MoveResult> RandomGamer::make_move(const AnotherGamer& callback_gamer) {
    return std::pair<Move, MoveResult>();
}

bool RandomGamer::lost() const {
    return false;
}

// Console gamer stuff

void ConsoleGamer::init(GameView& game_view) {

}

MoveResult ConsoleGamer::check_move(Move move) {
    return MoveResult::Destroyed;
}

std::pair<Move, MoveResult> ConsoleGamer::make_move(const AnotherGamer& callback_gamer) {
    return std::pair<Move, MoveResult>();
}

bool ConsoleGamer::lost() const {
    return false;
}
