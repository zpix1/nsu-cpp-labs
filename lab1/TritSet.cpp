#include "TritSet.h"

#include <iostream>
#include <algorithm>


TritSet::TritSet(const size_t length) :
        _length(length),
        _capacity((length * TritSet::TRIT_SIZE_IN_BITS + TritSet::INT_SIZE * 8 - 1) / (TritSet::INT_SIZE * 8)),
        arr(nullptr) {
    arr = static_cast<unsigned int *>(malloc(_capacity * TritSet::INT_SIZE));
    for (size_t i = 0; i < _capacity; i++) {
        arr[i] = 0;
    }
}


TritSet::TritSet(TritSet &&other) noexcept:
        _length(other._length),
        _capacity(other._capacity),
        arr(other.arr) {
    other._length = 0;
    other._capacity = 0;
    other.arr = nullptr;
}


Trit TritSet::get_trit(const size_t idx) const {
    if (idx >= _length) {
        return Trit::Unknown;
    }
    size_t cell_idx = TritSet::get_trit_cell_idx(idx);
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
        for (size_t i = _capacity; i < cell_idx + 1; i++) {
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

size_t TritSet::length() const {
    size_t cell_idx = 0;
    bool found_nonzero = false;
    for (size_t i = 0; i < _capacity; i++) {
        if (arr[_capacity - i - 1] != 0) {
            cell_idx = _capacity - i - 1;
            found_nonzero = true;
            break;
        }
    }

    size_t result_length = 0;

    if (!found_nonzero) {
        result_length = 0;
    } else {
        size_t temp_length = cell_idx * TritSet::TRITS_IN_INT;
        for (size_t i = 1; i <= TritSet::TRITS_IN_INT; i++) {
            if (get_trit(temp_length + TRITS_IN_INT - i) != Trit::Unknown) {
                result_length = temp_length + TRITS_IN_INT - i + 1;
                break;
            }
        }
    }
    return result_length;
}

TritSet TritSet::operator~() const {
    size_t len = _length;
    TritSet result{_capacity * TritSet::TRITS_IN_INT};
    for (size_t i = 0; i < len; i++) {
        result.set_trit(i, ~get_trit(i));
    }
    return result;
}

TritSet TritSet::operator&(const TritSet& b) const {
    size_t len = std::max(_length, b._length);
    size_t cap = std::max(_capacity, b._capacity);
    TritSet result{cap * TritSet::TRITS_IN_INT};
    for (size_t i = 0; i < len; i++) {
        result.set_trit(i, get_trit(i) & b.get_trit(i));
    }
    return result;
}

TritSet TritSet::operator|(const TritSet& b) const {
    size_t len = std::max(_length, b._length);
    size_t cap = std::max(_capacity, b._capacity);
    TritSet result{cap * TritSet::TRITS_IN_INT};
    for (size_t i = 0; i < len; i++) {
        result.set_trit(i, get_trit(i) | b.get_trit(i));
    }
    return result;
}

TritSet::~TritSet() {
    free(arr);
}

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
    size_t cell_idx = TritSet::get_trit_cell_idx(start_length - 1);
    if (start_length == 0) {
        arr = static_cast<unsigned int *>(realloc(arr, (0) * TritSet::INT_SIZE));
        _capacity = 0;
        _length = 0;
        return;
    }

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
    size_t new_capacity = TritSet::get_trit_cell_idx(new_length);
    arr = static_cast<unsigned int *>(realloc(arr, new_capacity * TritSet::INT_SIZE));
    _capacity = new_capacity;
    _length = length();
}

size_t TritSet::cardinality(Trit value) const {
    size_t cnt = 0;
    for (size_t i = 0; i < length(); i++) {
        if (get_trit(i) == value) {
            cnt++;
        }
    }
    return cnt;
}

std::unordered_map<Trit, size_t, std::hash<Trit>> TritSet::cardinality() const {
    std::unordered_map<Trit, size_t, std::hash<Trit>> result;
    for (size_t i = 0; i < length(); i++) {
        result[get_trit(i)]++;
    }
    return result;
}

TritSet::TritProxy TritSet::operator[](size_t idx) {
    return TritProxy{*this, idx};
}

Trit TritSet::operator[](size_t idx) const {
    return get_trit(idx);
}

TritSet::TritProxy::TritProxy(TritSet &ts, size_t idx) : ts(ts), idx(idx) {}

TritSet::TritProxy &TritSet::TritProxy::operator=(Trit value) {
    ts.set_trit(idx, value);
    return *this;
}

TritSet::TritProxy::operator Trit() {
    return ts.get_trit(idx);
}

Trit operator~(Trit t1) {
    if (t1 == Trit::True) {
        return Trit::False;
    }
    if (t1 == Trit::False) {
        return Trit::True;
    }
    return Trit::Unknown;
}

Trit operator&(Trit t1, Trit t2) {
    if (t1 == Trit::False || t2 == Trit::False) {
        return Trit::False;
    }
    if (t1 == Trit::Unknown || t2 == Trit::Unknown) {
        return Trit::Unknown;
    }
    return Trit::True;
}

Trit operator|(Trit t1, Trit t2) {
    if (t1 == Trit::True || t2 == Trit::True) {
        return Trit::True;
    }
    if (t1 == Trit::Unknown || t2 == Trit::Unknown) {
        return Trit::Unknown;
    }
    return Trit::False;
}
