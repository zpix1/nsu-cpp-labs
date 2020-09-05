//
// Created by John on 05.09.2020.
//

#include "TritSet.h"

#include <iostream>

TritSet::TritSet(const unsigned int length):
        _length(length),
        _capacity((length * TritSet::TRIT_SIZE_IN_BITS + TritSet::INT_SIZE * 8 - 1) / (TritSet::INT_SIZE * 8)),
        arr(nullptr) {
    arr = (int*)malloc(_capacity * TritSet::INT_SIZE);
    for (size_t i = 0; i < _capacity; i++) {
        arr[i] = 0;
    }
};

Trit TritSet::get_trit(const unsigned int idx) const {
    if (idx >= _length) {
        return Trit::Unknown;
    }
    size_t cell_idx = (idx * TritSet::TRIT_SIZE_IN_BITS) / (TritSet::INT_SIZE * 8);
    int cell_pos_in_bits = ( (idx * TritSet::TRIT_SIZE_IN_BITS) % (TritSet::INT_SIZE * 8) );
    int bit_value = (arr[cell_idx] >> cell_pos_in_bits) & 0b11;
    switch (bit_value) {
        case 0b00: return Trit::Unknown;
        case 0b01: return Trit::True;
        case 0b10: return Trit::False;
        default:   return Trit::Unknown; // Error, can't be
    }
}

void TritSet::set_trit(const unsigned int idx, const Trit value) {
    if (idx >= _length && value == Trit::Unknown) {
        return;
    }
    size_t cell_idx = (idx * TritSet::TRIT_SIZE_IN_BITS) / (TritSet::INT_SIZE * 8);
    if (cell_idx >= _capacity) {
        if (arr == nullptr) {
            arr = (int*)malloc((cell_idx + 1) * TritSet::INT_SIZE);
            _capacity = 0;
        } else {
            arr = (int*)realloc(arr, (cell_idx + 1) * TritSet::INT_SIZE);
        }
        for (size_t i = _capacity; i <= cell_idx + 1; i++) {
            arr[i] = 0;
        }
    }
    int cell_pos_in_bits = ( (idx * TritSet::TRIT_SIZE_IN_BITS) % (TritSet::INT_SIZE * 8) );
    int bit_value;
    switch (value) {
        case Trit::Unknown:
            bit_value = 0b00;
            break;
        case Trit::True:
            bit_value = 0b01;
            break;
        case Trit::False:
            bit_value = 0b10;
            break;
    }
    arr[cell_idx] &= 0x00;
}

size_t TritSet::capacity() const {
    return _capacity;
}

unsigned int TritSet::length() const {
    return _length;
}

// void TritSet::trim(unsigned int new_length);
// unsigned int TritSet::cardinality(Trit value) const;
// std::unordered_map< Trit, int, std::hash<int> > TritSet::cardinality();

// TritSet& TritSet::operator~() const;
// TritSet& TritSet::operator&(const TritSet &b) const;
// TritSet& TritSet::operator|(const TritSet &b) const;
// Trit& TritSet::operator[](const size_t idx);
TritSet::~TritSet() {};