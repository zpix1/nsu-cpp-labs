#include "Gamer.h"

void RandomGamer::init(EmptyGameView& gameview) {
    for (int i = 0; i < FIELD_SIZE; i++) {
        my_field.data.push_back(std::vector<BattlefieldCellState>(FIELD_SIZE, BattlefieldCellState::Empty));
        opponent_field.data.push_back(std::vector<BattlefieldCellState>(FIELD_SIZE, BattlefieldCellState::Unknown));
    }

    // !TODO: Place ships
}