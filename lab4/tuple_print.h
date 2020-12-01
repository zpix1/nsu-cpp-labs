#ifndef LAB4_TUPLE_PRINT_H
#define LAB4_TUPLE_PRINT_H

#include <istream>
#include <ostream>
#include <tuple>

template<typename T>
std::ostream& print(std::ostream& where, const T& what) {
    return where << what;
}

template<typename T, typename... Types>
std::ostream& print(std::ostream& where, const T& what, const Types& ... other) {
    return print(where << what << ' ', other...);
}

template<int...>
struct seq {};   // просто статический список чисел

template<int N, int... S> // генератор списка по типу Python'овского range()
struct make_range : make_range<N - 1, N - 1, S...> {};

template<int ...S>
struct make_range<0, S...> {
    typedef seq<S...> result;
};

template<typename... Types, int... range>
std::ostream& operator_shl_impl(
        std::ostream& out,
        const std::tuple<Types...>& what,
        const seq<range...> /* a dummy argument */) {
    return print(out, std::get<range>(what)...);
}

template<typename... Types>
std::ostream& operator<<(std::ostream& out, const std::tuple<Types...>& what) {
    using range = typename make_range<sizeof...(Types)>::result;
    return operator_shl_impl(out, what, range());
}

#endif //LAB4_TUPLE_PRINT_H
