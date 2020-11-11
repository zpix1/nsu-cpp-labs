#pragma once

#include <string>

#include "utils.h"

class GameView {
public:
    virtual void render_field(const Battlefield& field) = 0;

    virtual void log(const std::string& message) = 0;

    virtual ~GameView() = default;
};

class InteractiveGameView : public GameView {
public:
    virtual void clear() = 0;
    virtual Move ask_for_move() = 0;
    virtual std::pair<bool, int> confirm_ship_placement(Battlefield& field) = 0;
    virtual void pause() = 0;
};

class ConsoleGameView : public InteractiveGameView {
public:
    void render_field(const Battlefield& field) override;

    Move ask_for_move() override;
    std::pair<bool, int> confirm_ship_placement(Battlefield& field) override;
    void log(const std::string& message) override;

    void clear() override;

    void pause() override;
};