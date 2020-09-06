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
    static constexpr int INT_SIZE = sizeof(unsigned int);
    static constexpr int TRIT_SIZE_IN_BITS = 2;
    static constexpr int TRITS_IN_INT = TritSet::INT_SIZE * 8 / TritSet::TRIT_SIZE_IN_BITS;

    unsigned int *arr;
    size_t _capacity;
    unsigned int _length;

    static size_t get_trit_cell_idx(size_t idx);

    static size_t get_trit_cell_pos(size_t idx);

    class TritProxy {
        TritSet &ts;
        size_t idx;
    public:
        TritProxy(TritSet &ts, size_t idx);
        TritProxy& operator= (Trit value);
        operator Trit();
    };

public:
    // Set number bit to value (0 or 1)
    static unsigned int set_number_bit(unsigned int number, unsigned int idx, unsigned int value);

    Trit get_trit(size_t idx) const;

    void set_trit(size_t idx, Trit value);

    // Create TritSet by given length
    explicit TritSet(size_t length);
    ~TritSet();

    TritSet(TritSet &&other) noexcept;

    // Return internal array size in bytes
    size_t capacity() const;

    // Return index of the last non-Unknown trit + 1
    unsigned int length() const;

    // Forget about trits from new_length
    void trim(size_t new_length);

    // Reduce capacity for length
    void shrink();

    // Count trits of given value
    unsigned int cardinality(Trit value) const;

    // Count every trit type
    std::unordered_map<Trit, int, std::hash<Trit> > cardinality() const;

    TritSet operator~() const;

    TritSet operator&(const TritSet &b) const;

    TritSet operator|(const TritSet &b) const;

    TritProxy operator[](size_t idx);
};

#endif //LAB1_TRITSET_H
