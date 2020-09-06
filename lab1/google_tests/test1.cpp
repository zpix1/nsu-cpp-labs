#include "gtest/gtest.h"

#include "../TritSet.h"

TEST(TritSetTests, set_number_bit) {
    unsigned a = 0b101u;
    a = TritSet::set_number_bit(a, 2, 0);
    ASSERT_EQ(0b001u, a);
    a = TritSet::set_number_bit(a, 4, 1);
    ASSERT_EQ(0b10001u, a);
}

TEST(TritSetTests, length_capacity_shrink_works) {
    TritSet ts(100);
    ASSERT_EQ(7, ts.capacity());
    ASSERT_EQ(0, ts.length());
    ts.set_trit(9, Trit::True);
    ASSERT_EQ(7, ts.capacity());
    ASSERT_EQ(10, ts.length());
    ASSERT_EQ(Trit::True, ts.get_trit(9));
    ts.set_trit(1000, Trit::False);
    ASSERT_EQ(63, ts.capacity());
    ASSERT_EQ(1001, ts.length());
    ASSERT_EQ(Trit::True, ts.get_trit(9));
    ts.set_trit(1000, Trit::Unknown);
    ASSERT_EQ(63, ts.capacity());
    ASSERT_EQ(10, ts.length());
    ASSERT_EQ(Trit::True, ts.get_trit(9));
    ts.shrink();
    ASSERT_EQ(1, ts.capacity());
    ASSERT_EQ(10, ts.length());
    ASSERT_EQ(Trit::True, ts.get_trit(9));
}