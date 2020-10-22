#pragma once

#include <vector>
#include <utility>

#include "GameView.h"

const int FIELD_SIZE = 10;

enum class MoveResult {
    Missed, Hit, Destroyed
};

enum class BattlefieldCellState {
    Empty, Unknown, Locked, Hit, Destroyed, Ship
};

struct Move {
    int x, y;
};

struct Ship {
    int width, height;
};

using Ships = std::vector<Ship>;

struct Battlefield {
    int width, height;
    std::vector< std::vector <BattlefieldCellState> > data;
};

class AnotherGamer {
    MoveResult check_move(Move move);
};

class Gamer {
    virtual void init(GameView& gameview) = 0;
    
    virtual std::pair<Move, MoveResult> make_move(const AnotherGamer& callback_gamer) = 0;
    
    virtual bool lost() const = 0;
};

class RandomGamer: Gamer, AnotherGamer {
    Battlefield my_field;
    Battlefield opponent_field;

public:
    MoveResult check_move(Move move);
    
    void init(EmptyGameView& gameview);
    
    std::pair<Move, MoveResult> make_move(const AnotherGamer& callback_gamer);
    
    bool lost() const;
};

class ConsoleGamer: Gamer, AnotherGamer {
    Battlefield my_field;
    Battlefield opponent_field;

public:
    MoveResult check_move(Move move);
    
    void init(InteractiveGameView& gameview);
    
    std::pair<Move, MoveResult> make_move(const AnotherGamer& callback_gamer);

    bool lost() const;
};