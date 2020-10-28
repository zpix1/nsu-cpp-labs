#pragma once

#include <vector>
#include <utility>

#include "utils.h"
#include "GameView.h"

class AnotherGamer {
public:
    virtual MoveResult check_move(Move move) = 0;
};

class Gamer {
    virtual void init(GameView& game_view) = 0;

    virtual std::pair<Move, MoveResult> make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) = 0;

    [[nodiscard]] virtual bool lost() const = 0;
};

class UtilityGamer : public Gamer, public AnotherGamer {
public:
    int ships_count = SHIPS.size();
    Battlefield my_field;

    MoveResult check_move(Move move) override;

    [[nodiscard]] bool lost() const override;
};

class RandomGamer : public UtilityGamer {

public:

    void init(GameView& game_view) override;

    std::pair<Move, MoveResult> make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) override;
};

class ConsoleGamer : public UtilityGamer {
    Battlefield opponent_field;

public:
    void init(GameView& game_view) override;

    std::pair<Move, MoveResult> make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) override;
};