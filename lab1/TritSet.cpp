//
// Created by John on 05.09.2020.
//

#include "TritSet.h"

#include <iostream>


TritSet::TritSet(const size_t length) :
        _length(length),
        _capacity((length * TritSet::TRIT_SIZE_IN_BITS + TritSet::INT_SIZE * 8 - 1) / (TritSet::INT_SIZE * 8)),
        arr(nullptr) {
    arr = static_cast<unsigned int *>(malloc(_capacity * TritSet::INT_SIZE));
    for (size_t i = 0; i < _capacity; i++) {
        arr[i] = 0;
    }
};

Trit TritSet::get_trit(const size_t idx) const {
    if (idx >= _length) {
        return Trit::Unknown;
    }
    size_t cell_idx = TritSet::get_trit_cell_idx(idx);;
    unsigned int cell_pos_in_bits = TritSet::get_trit_cell_pos(idx);
    unsigned int bit_value = (arr[cell_idx] >> cell_pos_in_bits) & 0b11u;
    switch (bit_value) {
        case 0b00:
            return Trit::Unknown;
        case 0b01:
            return Trit::True;
        case 0b10:
            return Trit::False;
        default:
            return Trit::Unknown; // Error, can't be
    }
}

void TritSet::set_trit(const size_t idx, const Trit value) {
    if (idx >= _length && value == Trit::Unknown) {
        return;
    }
    size_t cell_idx = TritSet::get_trit_cell_idx(idx);
    if (cell_idx >= _capacity) {
        if (arr == nullptr) {
            arr = static_cast<unsigned int *>(malloc((cell_idx + 1) * TritSet::INT_SIZE));
            _capacity = 0;
        } else {
            arr = static_cast<unsigned int *>(realloc(arr, (cell_idx + 1) * TritSet::INT_SIZE));
        }
        for (size_t i = _capacity; i <= cell_idx + 1; i++) {
            arr[i] = 0;
        }
        _capacity = cell_idx + 1;
    }
    if (idx + 1 > _length) {
        _length = idx + 1;
    }
    unsigned int bit_value;
    switch (value) {
        case Trit::Unknown:
            bit_value = 0b00u;
            break;
        case Trit::True:
            bit_value = 0b01u;
            break;
        case Trit::False:
            bit_value = 0b10u;
            break;
    }

    int cell_pos_in_bits = TritSet::get_trit_cell_pos(idx);
    arr[cell_idx] = TritSet::set_number_bit(arr[cell_idx], cell_pos_in_bits, bit_value & 0b1u);
    arr[cell_idx] = TritSet::set_number_bit(arr[cell_idx], cell_pos_in_bits + 1, bit_value >> 1u);
}

size_t TritSet::capacity() const {
    return _capacity;
}

unsigned int TritSet::length() {
    size_t cell_idx = 0;
    bool found_nonzero = false;
    for (size_t i = 0; i < _capacity; i++) {
        if (arr[_capacity - i - 1] != 0) {
            cell_idx = _capacity - i - 1;
            found_nonzero = true;
            break;
        }
    }

    if (!found_nonzero) {
        _length = 0;
    } else {
        size_t temp_length = cell_idx * TritSet::TRITS_IN_INT;
        for (size_t i = 0; i < TritSet::TRITS_IN_INT; i++) {
            if (get_trit(temp_length + i) != Trit::Unknown) {
                _length = temp_length + i + 1;
                break;
            }
        }
    }

    return _length;
}

// void TritSet::trim(unsigned int new_length);
// unsigned int TritSet::cardinality(Trit value) const;
// std::unordered_map< Trit, int, std::hash<int> > TritSet::cardinality();

// TritSet& TritSet::operator~() const;
// TritSet& TritSet::operator&(const TritSet &b) const;
// TritSet& TritSet::operator|(const TritSet &b) const;
// Trit& TritSet::operator[](const size_t idx);
TritSet::~TritSet() {}

size_t TritSet::get_trit_cell_idx(size_t idx) {
    return (idx * TritSet::TRIT_SIZE_IN_BITS) / (TritSet::INT_SIZE * 8);
}

size_t TritSet::get_trit_cell_pos(size_t idx) {
    return ((idx * TritSet::TRIT_SIZE_IN_BITS) % (TritSet::INT_SIZE * 8));
}

unsigned int TritSet::set_number_bit(unsigned int number, unsigned int idx, unsigned int value) {
    number ^= (-(unsigned long) value ^ number) & (1UL << idx);
    return number;
}

void TritSet::shrink() {
    size_t start_length = length();
    if (start_length == 0) {
        return;
    }
    size_t cell_idx = TritSet::get_trit_cell_idx(start_length - 1);
    arr = static_cast<unsigned int *>(realloc(arr, (cell_idx + 1) * TritSet::INT_SIZE));

    size_t temp_length = cell_idx * TritSet::INT_SIZE * 8 / TritSet::TRIT_SIZE_IN_BITS;
    for (size_t i = 0; i < TritSet::INT_SIZE * 8 / TritSet::TRIT_SIZE_IN_BITS; i++) {
        if (get_trit(temp_length + i) != Trit::Unknown) {
            _length = temp_length + i + 1;
            break;
        }
    }

    _capacity = cell_idx + 1;
}

void TritSet::trim(size_t new_length) {

}
