#pragma once

#include <unordered_map>
#include <vector>
#include <utility>
#include <random>
#include <unordered_set>


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
    return mersenne();
}