#include "GameView.h"

#include <iostream>

void ConsoleGameView::render_field(const Battlefield& field) {
    std::cout << " ";
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

Move ConsoleGameView::ask_for_move() {
    log("Please enter x, y:");
    int x, y;
    while (true) {
        std::cin >> x >> y;
        if (is_valid_point(x, y)) {
            return Move{x, y};
        } else {
            log("Invalid x or y. Please try again:");
        }
    }
}

void ConsoleGameView::log(const std::string& message) {
    std::cout << message << std::endl;
}

void ConsoleGameView::clear() {
#if defined _WIN32
    system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined (__APPLE__)
    system("clear");
#endif
}

void ConsoleGameView::pause() {
    char c;
    std::cin >> c;
}

