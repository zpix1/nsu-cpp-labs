#include "Gamer.h"

void RandomGamer::init(const int width, const int height, const Ships& ships) {
    my_field.width = opponent_field.width = width;
    my_field.height = opponent_field.height = height;

    for (int i = 0; i < height; i++) {
        my_field.data.push_back(std::vector<BattlefieldCellState>(width, BattlefieldCellState::Empty));
        opponent_field.data.push_back(std::vector<BattlefieldCellState>(width, BattlefieldCellState::Unknown));
    }

    // !TODO: Place ships
}