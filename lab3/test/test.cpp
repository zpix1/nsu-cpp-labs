#define CATCH_CONFIG_MAIN

#include "../thirdparty/catch.hpp"
#include "../thirdparty/fakeit.hpp"

#include "../Gamer.h"
#include "../GameView.h"
#include "../utils.h"

using namespace fakeit;

auto E = BattlefieldCellState::Empty;
auto S = BattlefieldCellState::Ship;
auto H = BattlefieldCellState::Hit;
auto D = BattlefieldCellState::Destroyed;
auto L = BattlefieldCellState::Locked;

TEST_CASE("Utils", "[utils]") {
    SECTION("ship marking works") {
        SECTION("is_ship_alive works") {
            Battlefield fieldA = {
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, S, S, S, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {S, H, H, H, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
            };
            REQUIRE(is_ship_alive(fieldA, 4, 3) == true);
            Battlefield fieldB = {
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, S, S, S, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {H, H, H, H, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
            };
            REQUIRE(is_ship_alive(fieldB, 4, 3) == false);
        }
        SECTION("ship_destroy works unmarked") {
            Battlefield fieldA = {
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, S, S, S, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {H, H, H, H, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
            };
            ship_destroy(fieldA, 4, 3, false);
            Battlefield fieldB = {
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, S, S, S, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {D, D, D, D, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
            };
            REQUIRE(fieldA == fieldB);
        }
        SECTION("ship_destroy works marked") {
            Battlefield fieldA = {
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, S, S, S, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {H, H, H, H, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
            };
            ship_destroy(fieldA, 4, 3, true);
            Battlefield fieldB = {
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, S, S, S, E, E},
                    {L, L, L, L, L, E, E, E, E, E},
                    {D, D, D, D, L, E, E, E, E, E},
                    {L, L, L, L, L, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
            };
            REQUIRE(fieldA == fieldB);
        }
        SECTION("ship_mark works") {
            Battlefield fieldA = {
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
            ship_mark(fieldA, 4, 3);
            Battlefield fieldB = {
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, S, S, S, E, E},
                    {L, L, L, L, L, E, E, E, E, E},
                    {S, S, S, S, L, E, E, E, E, E},
                    {L, L, L, L, L, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, E, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
                    {E, E, E, E, E, E, E, E, S, E},
            };
            REQUIRE(fieldA == fieldB);
        }
    }
}

TEST_CASE("StrategyGamer", "[gamer]") {
    SECTION("it can win") {
        int sum = 0;

        for (int tryno = 0; tryno < 100; tryno++) {
            Mock<ConsoleGameView> mock;

            StrategyGamer smart;
            RandomGamer dummy;
            const auto[count, fieldA] = place_ships_randomly();

            smart.init(mock.get(), std::make_pair(0, Battlefield{}));
            smart.prepare(mock.get());

            dummy.init(mock.get(), std::make_pair(count, fieldA));
            dummy.prepare(mock.get());

            REQUIRE(dummy.lost() == false);

            int i = 0;
            while (i < 100) {
                auto[move, result] = smart.make_move(mock.get(), dummy);
                if (result != MoveResult::Missed) {
                    i--;
                }
                i++;

                if (dummy.lost()) {
                    break;
                }
            }

            REQUIRE(dummy.lost() == true);

            sum += i;
        }
        double mean = sum * 1.0 / 100;

        std::cout << "Strategy gamer mean shots to win " << mean << std::endl;

        REQUIRE(mean < 75);
    }
}