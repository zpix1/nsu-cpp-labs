#pragma once

#include <string>

#include "utils.h"

class GameView {
    virtual void render_field(const Battlefield& field) = 0;

    virtual void log(const std::string& message) = 0;
};

class EmptyGameView: GameView {};

class InteractiveGameView: public GameView {
    virtual Battlefield ask_for_ships_placement() = 0;

    virtual Move ask_for_move(const std::string& invite) = 0;
};

class ConsoleGameView: public InteractiveGameView {
public:
    void render_field(const Battlefield& field) override;

    Battlefield ask_for_ships_placement() override;

    Move ask_for_move(const std::string& invite) override;

    void log(const std::string& message) override;
};