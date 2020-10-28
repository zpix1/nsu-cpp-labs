#pragma once

#include <unordered_map>
#include <vector>
#include <utility>

const int FIELD_WIDTH = 10;
const int FIELD_HEIGHT = 10;

enum class MoveResult {
    Missed, Hit, Destroyed
};

enum class BattlefieldCellState {
    Empty, Unknown, Locked, Hit, Destroyed, Ship
};

struct Move {
    int x, y;
};

struct Ship {
    int width, height;
};

using Ships = std::vector<Ship>;

using Battlefield = std::vector<std::vector<BattlefieldCellState> >;

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


struct Direction {
    int x, y;

    bool operator==(const Direction& b) const {
        return x == b.x && y == b.y;
    }
};

namespace std {
    template<>
    struct hash<Direction> {
        size_t operator()(const Direction& obj) const {
            return hash<int>()(obj.x + obj.y * FIELD_HEIGHT);
        }
    };
}


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
        {BattlefieldCellState::Locked,    ' '}
};


inline bool is_valid_point(int x, int y) {
    if (x >= 0 && x < FIELD_HEIGHT)
        if (y >= 0 && y < FIELD_WIDTH)
            return true;
    return false;
}

inline bool is_valid_point(const Move& move) {
    return is_valid_point(move.x, move.y);
    if (move.x >= 0 && move.x < FIELD_HEIGHT)
        if (move.y >= 0 && move.y < FIELD_WIDTH)
            return true;
    return false;
}
