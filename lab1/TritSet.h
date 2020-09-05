//
// Created by John on 05.09.2020.
//

#ifndef LAB1_TRITSET_H
#define LAB1_TRITSET_H


#include <cstdlib>
#include <unordered_map>
#include <functional>

enum class Trit {
    False = -1,
    Unknown = 0,
    True = 1
};

class TritSet {

public:
    unsigned int* arr;
    size_t _capacity;
    unsigned int _length;

    static size_t get_trit_cell_idx(unsigned int idx);
    static size_t get_trit_cell_pos(unsigned int idx);
    static unsigned int set_number_bit(unsigned  int number, unsigned  int idx, unsigned  int value);

    Trit get_trit(unsigned int idx) const;
    void set_trit(unsigned int idx, Trit value);

    static constexpr int INT_SIZE = sizeof(unsigned int);
    static constexpr int TRIT_SIZE_IN_BITS = 2;
    static constexpr int TRITS_IN_INT = TritSet::INT_SIZE * 8 / TritSet::TRIT_SIZE_IN_BITS;

    // Create TritSet by given length
    explicit TritSet(unsigned int length);

    // Return internal array size in bytes
    size_t capacity() const;

    // Return index of the last non-Unknown trit + 1
    // Possibly change _length, but never change capacity
    unsigned int length();

    // Forget about trits from new_length
    void trim(unsigned int new_length);

    // Reduce capacity for length
    void shrink();

    // Count trits of given value
    unsigned int cardinality(Trit value) const;

    // Count every trit type
    std::unordered_map< Trit, int, std::hash<int> > cardinality();

    TritSet& operator~() const;
    TritSet& operator&(const TritSet &b) const;
    TritSet& operator|(const TritSet &b) const;
    Trit& operator[](const size_t idx);

    ~TritSet();
};

#endif //LAB1_TRITSET_H
