#define CATCH_CONFIG_MAIN

#include "../thirdparty/catch.hpp"
#include "../thirdparty/fakeit.hpp"

#include "../Gamer.h"
#include "../GameView.h"

using namespace fakeit;

auto E = BattlefieldCellState::Empty;
auto S = BattlefieldCellState::Ship;

TEST_CASE("StrategyGamer", "[gamer]") {
    SECTION("it can win") {
        Mock<ConsoleGameView> mock;

        StrategyGamer smart;
        RandomGamer dummy;
        const Battlefield fieldA = {
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, S, S, S, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {S, S, S, S, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, E, E},
                {E, E, E, E, E, E, E, E, S, E},
                {E, E, E, E, E, E, E, E, S, E},
                {E, E, E, E, E, E, E, E, S, E},
        };

        smart.init(mock.get(), std::make_pair(0, Battlefield{}));
        smart.prepare(<#initializer#>);

        dummy.init(mock.get(), std::make_pair(3, fieldA));
        dummy.prepare(<#initializer#>);

        REQUIRE(dummy.lost() == false);

        int i = 0;
        while (i < 40) {
            auto [move, result] = smart.make_move(mock.get(), dummy);
            if (result != MoveResult::Missed) {
                i--;
            }
            i++;

            if (dummy.lost()) {
                break;
            }
        }

        REQUIRE(dummy.lost() == true);
    }
}