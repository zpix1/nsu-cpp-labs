#pragma once

#include <unordered_map>
#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <unordered_set>
#include <functional>


const int FIELD_WIDTH = 10;
const int FIELD_HEIGHT = 10;

enum class MoveResult {
    Missed, Hit, Destroyed
};

enum class BattlefieldCellState {
    Empty, Unknown, Locked, Hit, Destroyed, Ship
};

struct Point {
    int x, y;

    bool operator==(const Point& b) const {
        return x == b.x && y == b.y;
    }
};

namespace std {
    template<>
    struct hash<Point> {
        size_t operator()(const Point& obj) const {
            return hash<int>()(obj.x + obj.y * FIELD_HEIGHT);
        }
    };
}
using Move = Point;
using Ship = Point;
using Direction = Point;

using Battlefield = std::vector<std::vector<BattlefieldCellState> >;
using Ships = std::vector<Ship>;

const Ships SHIPS = {
        {1, 4},
        {1, 3},
        {1, 3},
        {1, 2},
        {1, 2},
        {1, 2},
        {1, 1},
        {1, 1},
        {1, 1},
        {1, 1}
};

const std::vector<Direction> DIRECTIONS = {
        {1,  1},
        {1,  -1},
        {-1, 1},
        {-1, -1}
};

const std::vector<std::pair<int, int>> NEIGHBOURS = {
        {0,  1},
        {0,  -1},
        {1,  0},
        {-1, 0},
        {1,  1},
        {1,  -1},
        {-1, 1},
        {-1, -1}
};

const std::unordered_map<BattlefieldCellState, char> STATE2CHAR{
        {BattlefieldCellState::Ship,      '#'},
        {BattlefieldCellState::Empty,     ' '},
        {BattlefieldCellState::Unknown,   '?'},
        {BattlefieldCellState::Destroyed, 'D'},
        {BattlefieldCellState::Hit,       'H'},
        {BattlefieldCellState::Locked,    '_'}
};


inline bool is_valid_point(int x, int y) {
    if (x >= 0 && x < FIELD_HEIGHT)
        if (y >= 0 && y < FIELD_WIDTH)
            return true;
    return false;
}

inline bool is_valid_point(const Move& move) {
    return is_valid_point(move.x, move.y);
}

inline int randint() {
    static std::random_device rd;
    static std::mt19937 mersenne(rd());
    int result;
    while ((result = mersenne()) < 0);
    return result;
}

static inline void
ship_dfs_set(std::vector<std::vector<bool>>& used, Battlefield& field, int x, int y, BattlefieldCellState go_on_from,
             BattlefieldCellState go_on_to, BattlefieldCellState neigh_from, BattlefieldCellState neigh_to,
             const std::function<void(BattlefieldCellState)>& process_cell) {
    if (used[x][y])
        return;
    used[x][y] = true;
    if (field[x][y] == neigh_from) {
        field[x][y] = neigh_to;
        return;
    }
    process_cell(field[x][y]);
    if (field[x][y] != go_on_from)
        return;
    field[x][y] = go_on_to;
    for (const auto[x_neighbour, y_neighbour]: NEIGHBOURS) {
        int x_check = x + x_neighbour;
        int y_check = y + y_neighbour;
        if (is_valid_point(x_check, y_check)) {
            ship_dfs_set(used, field, x_check, y_check, go_on_from, go_on_to, neigh_from, neigh_to, process_cell);
        }
    }
}

static inline void
ship_set(Battlefield& field, int x, int y, BattlefieldCellState go_on_from, BattlefieldCellState go_on_to,
         BattlefieldCellState neigh_from, BattlefieldCellState neigh_to,
         const std::function<void(BattlefieldCellState)>& process_cell) {
    std::vector<std::vector<bool>> used(FIELD_HEIGHT, std::vector<bool>(FIELD_WIDTH));
    ship_dfs_set(used, field, x, y, go_on_from, go_on_to, neigh_from, neigh_to, process_cell);
}

// Ship is alive if only at least one of its cells is alive
inline bool is_ship_alive(Battlefield& field, int x, int y) {
    bool result = false;
    ship_set(field, x, y, BattlefieldCellState::Hit, BattlefieldCellState::Hit, BattlefieldCellState::Empty,
             BattlefieldCellState::Empty, [&result](const BattlefieldCellState state) {
                result = result || (state == BattlefieldCellState::Ship);
            });
    return result;
}

// Ship is alive if only at least one of its cells is alive
inline void ship_destroy(Battlefield& field, int x, int y, bool mark_locked = false) {
    if (mark_locked) {
        ship_set(field, x, y, BattlefieldCellState::Hit, BattlefieldCellState::Destroyed,
                 BattlefieldCellState::Empty, BattlefieldCellState::Locked, [](BattlefieldCellState) {});
    } else {
        ship_set(field, x, y, BattlefieldCellState::Hit, BattlefieldCellState::Destroyed,
                 BattlefieldCellState::Empty, BattlefieldCellState::Empty, [](BattlefieldCellState) {});
    }
}

// Mark ship
inline void ship_mark(Battlefield& field, int x, int y) {
    ship_set(field, x, y, BattlefieldCellState::Ship, BattlefieldCellState::Ship,
             BattlefieldCellState::Empty, BattlefieldCellState::Locked, [](BattlefieldCellState) {});
}

inline std::pair<int, Battlefield> place_ships_randomly() {
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
