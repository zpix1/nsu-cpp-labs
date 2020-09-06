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
    TritSet set(1000);
    // length of internal array
    size_t allocLength = set.capacity();
    assert(allocLength >= 1000 * 2 / 8 / sizeof(unsigned int));
    // 1000*2 - min bits count
    // 1000*2 / 8 - min bytes count
    // 1000*2 / 8 / sizeof(uint) - min uint[] size

    //не выделяет никакой памяти
    set[1000000000] = Trit::Unknown;
    ASSERT_EQ(allocLength, set.capacity());

    // false, but no exception or memory allocation
    if (set[2000000000] == Trit::True) {}
    ASSERT_EQ(allocLength, set.capacity());

    //выделение памяти
    set[1000000000] = Trit::True;
    ASSERT_LT(allocLength, set.capacity());

    // no memory operations
    allocLength = set.capacity();
    set[1000000000] = Trit::Unknown;
    set[1000000] = Trit::False;
    ASSERT_EQ(allocLength, set.capacity());

    // освобождение памяти до начального значения или
    // до значения необходимого для хранения последнего установленного трита
    // в данном случае для трита 1000’000
    set.shrink();
    ASSERT_GT(allocLength, set.capacity());
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

TEST(TritSetTests, not_overload) {
    TritSet ts1{0};
    ts1.set_trit(0, Trit::True);
    ts1.set_trit(1, Trit::False);
    ts1.set_trit(2, Trit::Unknown);
    ts1.set_trit(3, Trit::True);

    auto ts2 = ~ts1;
    ASSERT_EQ(Trit::False, ts2.get_trit(0));
    ASSERT_EQ(Trit::True, ts2.get_trit(1));
    ASSERT_EQ(Trit::Unknown, ts2.get_trit(2));
    ASSERT_EQ(Trit::False, ts2.get_trit(3));
}

TEST(TritSetTests, and_overload) {
    TritSet ts1{0};
    ts1.set_trit(0, Trit::True);
    ts1.set_trit(1, Trit::False);
    ts1.set_trit(2, Trit::Unknown);
    ts1.set_trit(3, Trit::True);

    TritSet ts2{0};
    ts2.set_trit(0, Trit::False);
    ts2.set_trit(1, Trit::False);
    ts2.set_trit(2, Trit::False);
    ts2.set_trit(3, Trit::True);

    ts2.set_trit(100, Trit::False);

    auto ts3 = ts1 & ts2;

    ASSERT_EQ(Trit::False, ts3.get_trit(0));
    ASSERT_EQ(Trit::False, ts3.get_trit(1));
    ASSERT_EQ(Trit::False, ts3.get_trit(2));
    ASSERT_EQ(Trit::True, ts3.get_trit(3));

    ASSERT_EQ(Trit::False, ts3.get_trit(100));
}

TEST(TritSetTests, or_overload) {
    TritSet ts1{0};
    ts1.set_trit(0, Trit::True);
    ts1.set_trit(1, Trit::False);
    ts1.set_trit(2, Trit::Unknown);
    ts1.set_trit(3, Trit::False);

    TritSet ts2{0};
    ts2.set_trit(0, Trit::False);
    ts2.set_trit(1, Trit::False);
    ts2.set_trit(2, Trit::False);
    ts2.set_trit(3, Trit::False);

    ts2.set_trit(100, Trit::True);

    auto ts3 = ts1 | ts2;

    ASSERT_EQ(Trit::True, ts3.get_trit(0));
    ASSERT_EQ(Trit::False, ts3.get_trit(1));
    ASSERT_EQ(Trit::Unknown, ts3.get_trit(2));
    ASSERT_EQ(Trit::False, ts3.get_trit(3));

    ASSERT_EQ(Trit::True, ts3.get_trit(100));
}

TEST(TritSetTests, and_capacity_extend) {
    TritSet setA(1000);
    TritSet setB(2000);
    TritSet setC = setA & setB;
    ASSERT_EQ(setC.capacity(), setB.capacity());
}