#ifndef LAB4_CSVPARSER_H
#define LAB4_CSVPARSER_H

template<class ...Types> class CSVParser {
    std::istream& ifs;
    unsigned int lines_to_skip;
    bool lines_skipped = false;
public:
    explicit CSVParser(std::istream& ifs, const int lines_to_skip = 0) : ifs(ifs), lines_to_skip(lines_to_skip) {}
};

#endif //LAB4_CSVPARSER_H
