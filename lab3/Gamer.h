#pragma once

#include <vector>
#include <unordered_set>
#include <utility>

#include "utils.h"
#include "GameView.h"

class AnotherGamer;

class Gamer {
public:
    virtual std::pair<Move, MoveResult> make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) = 0;

    [[nodiscard]] virtual bool lost() const = 0;

    virtual void init(GameView& game_view) = 0;
};

class AnotherGamer: Gamer {
public:
    virtual MoveResult check_move(Move move) = 0;
};


class UtilityGamer : public AnotherGamer {
public:
    int ships_count = 0;
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

class StrategyGamer : public UtilityGamer {
    enum class State {
        Search,
        IdentifyDirection,
        FinishOff
    };

    std::vector<int> field_cell_order;

    State state = State::Search;


    Move first_hit;
    Move current_hit;

    const std::unordered_set<Direction> all_directions = {
            {0, 1},
            {0, -1},
            {1, 0},
            {-1, 0}
    };

    std::unordered_set<Direction> possible_directions = all_directions;

    Direction current_direction;

public:
    Battlefield opponent_field;
    void init(GameView& game_view) override;

    std::pair<Move, MoveResult> make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) override;
};