#ifndef LAB4_CSVPARSER_H
#define LAB4_CSVPARSER_H

#include <iterator>
#include <utility>
#include <iostream>
#include <sstream>

namespace CSVParser {
    class ParsingException : public std::exception {
        std::string error_message;
        unsigned int line_idx;
        unsigned int char_idx;
    public:
        explicit ParsingException(std::string msg, const int nline, const int nchar) :
                error_message(std::move(msg)),
                line_idx(nline),
                char_idx(nchar) {};

        [[nodiscard]] inline const char *what() const noexcept override {
            return error_message.c_str();
        }

        [[nodiscard]] auto get_line_idx() const {
            return line_idx;
        }

        [[nodiscard]] auto get_char_idx() const {
            return char_idx;
        }
    };

    template<class ...Types>
    struct CSVParserIterator;

    template<class ...Types>
    class CSVParser {
        unsigned int lines_to_skip;
        bool lines_skipped = false;
        bool is_done = false;
        unsigned int current_line_idx = 0;

        char delimiter;
        char escape;

        std::istream& ifs;
        std::tuple<Types...> current_value;

        template<class T>
        void read(std::stringstream& linestream, T& where);

        void next();

        std::tuple<Types...>& value();

        bool done();

    public:
        explicit CSVParser(std::istream& ifs,
                           const int lines_to_skip = 0,
                           const char delimiter = ',',
                           const char escape = '"') :
                ifs(ifs),
                lines_to_skip(lines_to_skip),
                delimiter(delimiter),
                escape(escape) {}

        friend CSVParserIterator<Types...>;
    };

    template<class T>
    bool read_from_to(std::stringstream& stream, T& to) {
        return static_cast<bool>(stream >> to);
    }

    bool read_from_to(std::stringstream& stream, std::string& to) {
        std::getline(stream, to);
        return static_cast<bool>(stream);
    }

    template<class... Types>
    template<class T>
    void CSVParser<Types...>::read(std::stringstream& linestream, T& where) {
        int start_idx = linestream.tellg();

        char c;
        linestream >> c;
        if (c != escape) {
            linestream.putback(c);
        }

        std::string scanstring;
        std::getline(linestream, scanstring, c == escape ? escape : delimiter);
        std::stringstream scanner{scanstring};
        if (!read_from_to(scanner, where)) {
            throw ParsingException("incompatible type", current_line_idx, start_idx + 1);
        }
        if (!scanner.eof()) {
            throw ParsingException("unexpected garbage at the end of the cell", current_line_idx,
                                   static_cast<int>(scanner.tellg()) + start_idx + 1);
        }
        linestream.ignore(1);
    }

    template<class... Types>
    void CSVParser<Types...>::next() {
        if (!lines_skipped) {
            for (int i = 0; i < lines_to_skip; i++) {
                ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            lines_skipped = true;
            current_line_idx += lines_skipped;
        }

        if (ifs.peek() == std::char_traits<char>::eof()) {
            is_done = true;
            return;
        }

        std::string line;
        std::getline(ifs, line);
        std::stringstream linestream{line};

        std::apply([&linestream, this](Types& ... args) {
            (this->read(linestream, args), ...);
        }, current_value);

        current_line_idx++;
    }

    template<class... Types>
    std::tuple<Types...>& CSVParser<Types...>::value() {
        return current_value;
    }

    template<class... Types>
    bool CSVParser<Types...>::done() {
        return is_done;
    }

    template<class ...Types>
    struct CSVParserIterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = std::tuple<Types...>;
        using pointer = value_type *;
        using reference = value_type&;

        using iterator = CSVParserIterator<Types...>;
        CSVParser<Types...> *generator;

        CSVParserIterator(CSVParser<Types...> *generator) : generator(generator) {}

        CSVParserIterator(const iterator&) = default;        // CopyConstructible
        iterator& operator=(const iterator&) = default;      // CopyAssignable
        ~CSVParserIterator() = default;                      // Destructible
        friend void swap(iterator& a, iterator& b) {         // Swappable
            std::swap(a.generator, b.generator);
        }

        // InputIterator requirements:
        friend bool operator==(const iterator& lhs, const iterator& rhs) {
            return lhs.generator == rhs.generator;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs) {
            return !(lhs == rhs);
        }

        reference operator*() const {
            return generator->value();
        }

        pointer operator->() const {
            return std::addressof(**this);
        }

        iterator& operator++() {
            generator->next();
            if (generator->done()) generator = nullptr;
            return *this;
        }

        struct CSVParserIteratorProxy {
            value_type current_value;

            CSVParserIteratorProxy(value_type& value) : current_value(std::move(value)) {}

            reference operator*() {
                return current_value;
            }
        };

        CSVParserIteratorProxy operator++(int) {
            CSVParserIteratorProxy old{**this};
            ++*this;
            return old;
        }
    };

    template<class ...Types>
    CSVParserIterator<Types...> begin(CSVParser<Types...>& generator) {
        CSVParserIterator<Types...> it{&generator};
        return ++it;
    }

    template<class ...Types>
    CSVParserIterator<Types...> end(CSVParser<Types...>&) {
        return {nullptr};
    }
};
#endif //LAB4_CSVPARSER_H
