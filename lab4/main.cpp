#include <iostream>
#include <utility>
#include <tuple>
#include <string>

#include "tuple_print.h"

int main() {
    std::tuple<int, std::tuple<int, std::string> > t{1, {1, "test"}};

    std::cout << t << std::endl;
    return 0;
}
