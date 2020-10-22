#pragma once

#include <string>

#include "Gamer.h"

class GameView {
    virtual void render_field(const Battlefield& field) = 0;
    virtual void log(const std::string& message) = 0;
};

class EmptyGameView: private GameView {};

class InteractiveGameView: GameView {
    virtual Battlefield ask_for_ships_placement() = 0;
    virtual Move ask_for_move(const std::string& invite) = 0;
};

class ConsoleGameView: InteractiveGameView {
    void render_field(const Battlefield& field);
    Battlefield ask_for_ships_placement();
    Move ask_for_move(const std::string& invite);
    void log(const std::string& message);
};