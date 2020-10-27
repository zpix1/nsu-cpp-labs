#include "GameView.h"

#include <iostream>

void ConsoleGameView::render_field(const Battlefield& field) {
    std::cout << "  ";
    for (int j = 0; j < FIELD_WIDTH; j++) {
        std::cout << j;
    }
    std::cout << std::endl;
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        std::cout << i;
        for (int j = 0; j < FIELD_WIDTH; j++) {
            std::cout << STATE2CHAR.at(field[i][j]);
        }
        std::cout << std::endl;
    }
}

Battlefield ConsoleGameView::ask_for_ships_placement() {
    return Battlefield();
}

Move ConsoleGameView::ask_for_move(const std::string& invite) {
    return Move();
}

void ConsoleGameView::log(const std::string& message) {
    std::cout << message << std::endl;
}

