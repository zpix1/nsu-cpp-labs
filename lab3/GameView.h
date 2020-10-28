#pragma once

#include <string>

#include "utils.h"

class GameView {
public:
    virtual void render_field(const Battlefield& field) = 0;

    virtual void log(const std::string& message) = 0;
};

class EmptyGameView : GameView {
};

class InteractiveGameView : public GameView {
public:
    virtual void clear() = 0;
    virtual Move ask_for_move() = 0;
    virtual void pause() = 0;
};

class ConsoleGameView : public InteractiveGameView {
public:
    void render_field(const Battlefield& field) override;

    Move ask_for_move() override;

    void log(const std::string& message) override;

    void clear() override;

    void pause() override;
};