#define CATCH_CONFIG_MAIN

#include "../thirdparty/catch.hpp"

#include "../Gamer.h"
#include "../GameView.h"

auto E = BattlefieldCellState::Empty;
auto S = BattlefieldCellState::Ship;

TEST_CASE("StrategyGamer", "[gamer]") {
    SECTION("it destroys ships") {
        StrategyGamer smart;
        RandomGamer dumb;
        const Battlefield fieldA = {
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {S, S, S, S, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
        };
        smart.opponent_field = fieldA;
        dumb.my_field = fieldA;
        for (int i = 0; i < 100; i++) {
            // TODO: Add mocking

            // smart.make_move(EmptyGameView(), dumb);
        }
    }
}