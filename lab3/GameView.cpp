#include "GameView.h"

#include <iostream>
#define DEBUG(x) do { std::cerr << #x << " = " << x << std::endl; } while (0)

void ConsoleGameView::render_field(const Battlefield& field) {
    std::cout << " ";
    for (int j = 0; j < FIELD_WIDTH; j++) {
        std::cout << j;
    }
    std::cout << std::endl;
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        std::cout << i;
        for (int j = 0; j < FIELD_WIDTH; j++) {
            std::cout << STATE2CHAR.at(field[i][j]);
        }
        std::cout << std::endl;
    }
}

Move ConsoleGameView::ask_for_move() {
    log("Please enter x, y:");
    int x, y;
    while (true) {
        std::cin >> x >> y;
        if (is_valid_point(x, y)) {
            return Move{x, y};
        } else {
            log("Invalid x or y. Please try again:");
        }
    }
}

void ConsoleGameView::log(const std::string& message) {
    std::cout << message << std::endl;
}

void ConsoleGameView::clear() {
#if defined _WIN32
    system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined (__APPLE__)
    system("clear");
#endif
}

void ConsoleGameView::pause() {
    system("pause");
}

static auto letter_to_rotation_and_direction(char direction) {
    if (direction == 'D')
        return std::make_pair(1, Direction{1, -1});
    if (direction == 'R')
        return std::make_pair(0, Direction{1, 1});
    if (direction == 'U')
        return std::make_pair(1, Direction{-1, 1});
    if (direction == 'L')
        return std::make_pair(0, Direction{-1, -1});
    return std::make_pair(0, Direction{0, 0});
}

static void ship_dfs_mark(std::vector<std::vector<bool>>& used, Battlefield& field, int x, int y) {
    if (used[x][y])
        return;
    used[x][y] = true;
    if (field[x][y] == BattlefieldCellState::Empty) {
        field[x][y] = BattlefieldCellState::Locked;
        return;
    }
    if (field[x][y] != BattlefieldCellState::Ship)
        return;
    for (const auto[x_neighbour, y_neighbour]: NEIGHBOURS) {
        int x_check = x + x_neighbour;
        int y_check = y + y_neighbour;
        if (is_valid_point(x_check, y_check)) {
            ship_dfs_mark(used, field, x_check, y_check);
        }
    }
}

// Ship is alive if only at least one of its cells is alive
static void ship_mark(Battlefield& field, int x, int y) {
    std::vector<std::vector<bool>> used(FIELD_HEIGHT, std::vector<bool>(FIELD_WIDTH));
    ship_dfs_mark(used, field, x, y);
}

static bool try_place_ship(Battlefield& field, Ship ship, int x, int y, Direction direction) {
    bool can_place = true;
    for (int x_part_pos = x;
         x_part_pos != x + direction.x * ship.x; x_part_pos += direction.x) {
        for (int y_part_pos = y;
             y_part_pos != y + direction.y * ship.y; y_part_pos += direction.y) {
            if (is_valid_point(x_part_pos, y_part_pos) &&
                field[x_part_pos][y_part_pos] == BattlefieldCellState::Empty) {
                for (const auto[x_neighbour, y_neighbour]: NEIGHBOURS) {
                    int x_check = x_part_pos + x_neighbour;
                    int y_check = y_part_pos + y_neighbour;

                    if (is_valid_point(x_check, y_check)) {
                        if (field[x_check][y_check] != BattlefieldCellState::Empty && field[x_check][y_check] != BattlefieldCellState::Locked) {
                            can_place = false;
                            goto RESULT_FOUND;
                        }
                    }
                }
            } else {
                can_place = false;
                goto RESULT_FOUND;
            }
        }
    }
    RESULT_FOUND:;
    if (can_place) {
        for (int x_part_pos = x;
             x_part_pos != x + direction.x * (ship.x); x_part_pos += direction.x) {
            for (int y_part_pos = y;
                 y_part_pos != y + direction.y * (ship.y); y_part_pos += direction.y) {
                field[x_part_pos][y_part_pos] = BattlefieldCellState::Ship;
            }
        }
    }
    return can_place;
}

int ConsoleGameView::confirm_ship_placement(Battlefield& field) {
    std::cout << "Ships were placed randomly: " << std::endl;
    render_field(field);
    std::cout << "Do you want to place ships manually? (Y/N): ";

    char result;
    std::cin >> result;
    if (result != 'Y')
        return false;

    for (int i = 0; i < FIELD_HEIGHT; i++) {
        for (int j = 0; j < FIELD_HEIGHT; j++) {
            field[i][j] = BattlefieldCellState::Empty;
        }
    }

    Ships available_ships = SHIPS;
    while (!available_ships.empty()) {
        std::cout << "Current field: " << std::endl;
        render_field(field);

        int id = 0;
        std::cout << "Ships: " << std::endl;
        for (const auto& ship: available_ships) {
            std::cout << "Ship " << id + 1 << ": " << ship.x << " " << ship.y << std::endl;
            id++;
        }

        bool stop = false;
        int user_x, user_y;
        int user_id;
        char user_direction_letter;

        std::cout << "Which ship and where do you want to place it? (Index [0 to stop], X, Y, Orientation [(U)p, (D)own, (L)eft, (R)ight]):";
        while (true) {
            std::cin >> user_id >> user_x >> user_y >> user_direction_letter;

            if (user_id == 0) {
                stop = true;
                break;
            }
            user_id--;

            auto [rotation, user_direction] = letter_to_rotation_and_direction(user_direction_letter);

            Ship ship;
            if (!(0 <= user_id && user_id < id)) {
                std::cout << "Invalid index. ";
            } else if (user_direction.x == 0 && user_direction.y == 0) {
                std::cout << "Invalid orientation. ";
            } else {
                ship = available_ships[user_id];

                if (rotation) {
                    std::swap(ship.x, ship.y);
                }
                bool set_result = try_place_ship(field, ship, user_x, user_y, user_direction);
                if (!set_result) {
                    std::cout << "Invalid position. ";
                } else {
                    available_ships.erase(available_ships.begin() + user_id);
                    ship_mark(field, user_x, user_y);
                    break;
                }
            }
            std::cout << "Please, try again. Which ship and where do you want to place it? (Index [0 to stop], X, Y, Orientation [(U)p, (D)own, (L)eft, (R)ight]):";
        }
        if (stop) {
            break;
        }
    }

    for (int i = 0; i < FIELD_HEIGHT; i++) {
        for (int j = 0; j < FIELD_HEIGHT; j++) {
            if (field[i][j] == BattlefieldCellState::Locked) {
                field[i][j] = BattlefieldCellState::Empty;
            }
        }
    }

    return SHIPS.size() - available_ships.size();
}


