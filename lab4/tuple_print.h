#ifndef LAB4_TUPLE_PRINT_H
#define LAB4_TUPLE_PRINT_H

#include <istream>
#include <ostream>
#include <tuple>

template<class... Types>
std::ostream& operator<<(std::ostream& os, std::tuple<Types...> const& t) {
    std::apply([&os](Types const& ... args) {
        os << '[';
        std::size_t n = 0;
        ((os << args << (++n != sizeof...(Types) ? ", " : "")), ...);
        os << ']';
    }, t);
    return os;
}

#endif //LAB4_TUPLE_PRINT_H
