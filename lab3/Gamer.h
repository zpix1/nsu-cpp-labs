#include <vector>
#include <utility>

enum class MoveResult {
    Hit, Missed, Destroyed
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
    virtual void init(int width, int height, const Ships& ships) = 0;
    virtual std::pair<Move, MoveResult> make_move(const AnotherGamer& callback_gamer) = 0;
};

class RandomGamer: Gamer, AnotherGamer {
    Battlefield my_field;
    Battlefield opponent_field;

    MoveResult check_move(Move move);
    void init(int width, int heigth, const Ships& ships);
    std::pair<Move, MoveResult> make_move(const AnotherGamer& callback_gamer);
};