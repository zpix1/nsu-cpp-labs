#pragma once

#include <vector>
#include <unordered_set>
#include <utility>

#include "utils.h"
#include "GameView.h"

std::pair<int, Battlefield> place_ships_randomly();

class AnotherGamer;

class Gamer {
public:
    virtual std::pair<Move, MoveResult> make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) = 0;

    [[nodiscard]] virtual bool lost() const = 0;

    virtual void init(GameView& game_view, std::pair<int, Battlefield> ships_count_with_field) = 0;

    virtual void prepare(InteractiveGameView& game_view) = 0;
};

class AnotherGamer : Gamer {
public:
    virtual MoveResult check_move(Move move) = 0;
};


class UtilityGamer : public AnotherGamer {
protected:
    int ships_count = 0;
    Battlefield my_field;

public:
    MoveResult check_move(Move move) override;

    [[nodiscard]] bool lost() const override;

    void init(GameView& game_view, std::pair<int, Battlefield> ships_count_with_field) override;
};

class RandomGamer : public UtilityGamer {

public:
    void prepare(InteractiveGameView& game_view) override {};

    std::pair<Move, MoveResult> make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) override;
};

class ConsoleGamer : public UtilityGamer {
    Battlefield opponent_field;

public:
    void prepare(InteractiveGameView& game_view) override;

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
            {0,  1},
            {0,  -1},
            {1,  0},
            {-1, 0}
    };

    std::unordered_set<Direction> possible_directions = all_directions;

    Direction current_direction;

    Battlefield opponent_field;
public:
    void prepare(InteractiveGameView& game_view) override;

    std::pair<Move, MoveResult> make_move(InteractiveGameView& game_view, AnotherGamer& callback_gamer) override;
};