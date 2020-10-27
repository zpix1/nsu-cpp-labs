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

//struct Battlefield {
//    std::vector< std::vector <BattlefieldCellState> > data;
//};

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

const std::vector<std::pair<int, int>> DIRECTIONS = {
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


