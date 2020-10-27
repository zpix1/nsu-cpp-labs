#pragma once

#include <vector>
#include <utility>

#include "utils.h"
#include "GameView.h"

class AnotherGamer {
    virtual MoveResult check_move(Move move) = 0;
};

class Gamer {
    virtual void init(GameView& game_view) = 0; // ?
    
    virtual std::pair<Move, MoveResult> make_move(const AnotherGamer& callback_gamer) = 0;
    
    virtual bool lost() const = 0;
};

class RandomGamer: public Gamer, public AnotherGamer {

public:
    Battlefield my_field;
    MoveResult check_move(Move move) override;
    
    void init(GameView& game_view) override; // ?
    
    std::pair<Move, MoveResult> make_move(const AnotherGamer& callback_gamer) override;
    
    bool lost() const override;
};

class ConsoleGamer: public Gamer, public AnotherGamer {
    Battlefield my_field;
    Battlefield opponent_field;

public:
    MoveResult check_move(Move move) override;
    
    void init(GameView& game_view) override; // ?
    
    std::pair<Move, MoveResult> make_move(const AnotherGamer& callback_gamer) override;

    bool lost() const override;
};