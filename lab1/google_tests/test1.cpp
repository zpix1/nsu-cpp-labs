#include "gtest/gtest.h"

#include "../TritSet.h"

TEST(TritSetTests, set_number_bit) {
    unsigned a = 0b101u;
    a = TritSet::set_number_bit(a, 2, 0);
    ASSERT_EQ(0b001u, a);
    a = TritSet::set_number_bit(a, 4, 1);
    ASSERT_EQ(0b10001u, a);
}

TEST(TritSetTests, doc_tests) {
    GTEST_SKIP_("doc tests are too slow");
    TritSet set(1000);
    size_t allocLength = set.capacity();
    ASSERT_TRUE(allocLength >= 1000 * 2 / 8 / sizeof(unsigned int));
    set.set_trit(1000000000, Trit::Unknown);
    ASSERT_TRUE(allocLength == set.capacity());
    if (set.get_trit(2000000000) == Trit::True) {}
    ASSERT_TRUE(allocLength == set.capacity());
    set.set_trit(1000000000, Trit::True);
    ASSERT_TRUE(allocLength < set.capacity());
    allocLength = set.capacity();
    set.set_trit(1000000000, Trit::Unknown);
    set.set_trit(1000000, Trit::False);
    ASSERT_TRUE(allocLength == set.capacity());
    set.shrink();
    ASSERT_TRUE(allocLength > set.capacity());
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

TEST(TritSetTests, trim) {
    TritSet ts(100);
    ASSERT_EQ(7, ts.capacity());
    ts.set_trit(50, Trit::True);
    ts.set_trit(70, Trit::False);
    ts.trim(65);
    ASSERT_EQ(Trit::Unknown, ts.get_trit(70));
    ASSERT_EQ(Trit::True, ts.get_trit(50));
    ASSERT_EQ(4, ts.capacity());
}