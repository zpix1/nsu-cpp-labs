#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../TritSet.h"

TEST_CASE("set_number_bit") {
    unsigned a = 0b101u;
    a = TritSet::set_number_bit(a, 2, 0);
    REQUIRE(0b001u == a);
    a = TritSet::set_number_bit(a, 4, 1);
    REQUIRE(0b10001u == a);
}

TEST_CASE("doc_tests") {
    TritSet set{1000};
    // length of internal array
    size_t allocLength = set.capacity();
    assert(allocLength >= 1000 * 2 / 8 / sizeof(unsigned int));
    // 1000*2 - min bits count
    // 1000*2 / 8 - min bytes count
    // 1000*2 / 8 / sizeof(uint) - min uint[] size

    // не выделяет никакой памяти
    set[1000000000] = Trit::Unknown;
    REQUIRE(allocLength == set.capacity());

    // false, but no exception or memory allocation
    if (set[2000000000] == Trit::True) {}
    REQUIRE(allocLength == set.capacity());

    //выделение памяти
    set[1000000000] = Trit::True;
    REQUIRE(allocLength < set.capacity());

    // no memory operations
    allocLength = set.capacity();
    set[1000000000] = Trit::Unknown;
    set[1000000] = Trit::False;
    REQUIRE(allocLength == set.capacity());

    // освобождение памяти до начального значения или
    // до значения необходимого для хранения последнего установленного трита
    // в данном случае для трита 1000’000
    set.shrink();
    REQUIRE(allocLength > set.capacity());
}

TEST_CASE("length_capacity_shrink") {
    TritSet ts{100};
    REQUIRE(7 == ts.capacity());
    REQUIRE(0 == ts.length());
    ts.set_trit(9, Trit::True);
    REQUIRE(7 == ts.capacity());
    REQUIRE(10 == ts.length());
    REQUIRE(Trit::True == ts.get_trit(9));
    ts.set_trit(1000, Trit::False);
    REQUIRE(63 == ts.capacity());
    REQUIRE(1001 == ts.length());
    REQUIRE(Trit::True == ts.get_trit(9));
    ts.set_trit(1000, Trit::Unknown);
    REQUIRE(63 == ts.capacity());
    REQUIRE(10 == ts.length());
    REQUIRE(Trit::True == ts.get_trit(9));
    ts.shrink();
    REQUIRE(1 == ts.capacity());
    REQUIRE(10 == ts.length());
    REQUIRE(Trit::True == ts.get_trit(9));
    ts.set_trit(1001, Trit::True);
    REQUIRE(1002 == ts.length());

}

TEST_CASE("trim") {
    TritSet ts{100};
    REQUIRE(7 == ts.capacity());
    ts.set_trit(50, Trit::True);
    ts.set_trit(70, Trit::False);
    ts.trim(65);
    REQUIRE(Trit::Unknown == ts.get_trit(70));
    REQUIRE(Trit::True == ts.get_trit(50));
    REQUIRE(4 == ts.capacity());
}

TEST_CASE("not_overload") {
    TritSet ts1{0};
    ts1.set_trit(0, Trit::True);
    ts1.set_trit(1, Trit::False);
    ts1.set_trit(2, Trit::Unknown);
    ts1.set_trit(3, Trit::True);

    auto ts2 = ~ts1;
    REQUIRE(Trit::False == ts2.get_trit(0));
    REQUIRE(Trit::True == ts2.get_trit(1));
    REQUIRE(Trit::Unknown == ts2.get_trit(2));
    REQUIRE(Trit::False == ts2.get_trit(3));
}

TEST_CASE("and_overload") {
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

    REQUIRE(Trit::False == ts3.get_trit(0));
    REQUIRE(Trit::False == ts3.get_trit(1));
    REQUIRE(Trit::False == ts3.get_trit(2));
    REQUIRE(Trit::True == ts3.get_trit(3));

    REQUIRE(Trit::False == ts3.get_trit(100));
}

TEST_CASE("or_overload") {
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

    REQUIRE(Trit::True == ts3.get_trit(0));
    REQUIRE(Trit::False == ts3.get_trit(1));
    REQUIRE(Trit::Unknown == ts3.get_trit(2));
    REQUIRE(Trit::False == ts3.get_trit(3));

    REQUIRE(Trit::True == ts3.get_trit(100));
}

TEST_CASE("and_capacity_extend") {
    TritSet setA(1000);
    TritSet setB(2000);
    TritSet setC = setA & setB;
    REQUIRE(setC.capacity() == setB.capacity());
}

TEST_CASE("cardinality_single") {
    TritSet ts{1000};
    REQUIRE(63 == ts.capacity());
    ts[1011] = Trit::True;
    ts[1012] = Trit::False;
    REQUIRE(1013 == ts.length());

    REQUIRE(1 == ts.cardinality(Trit::True));
    REQUIRE(1 == ts.cardinality(Trit::False));
    REQUIRE(1011 == ts.cardinality(Trit::Unknown));
}

TEST_CASE("cardinality_multi") {
    TritSet ts{1000};
    ts[1011] = Trit::True;
    ts[1012] = Trit::False;
    ts[1112] = Trit::True;
    ts[1312] = Trit::False;
    ts[1412] = Trit::True;

    auto mp = ts.cardinality();
    REQUIRE(3 == mp[Trit::True]);
    REQUIRE(2 == mp[Trit::False]);
    REQUIRE(1408 == mp[Trit::Unknown]);
}

TEST_CASE("set_get_not_and_or") {
    TritSet a{0};
    for (int i = 0; i < 1000; i++) {
        a[i] = Trit::False;
    }
    for (int i = 0; i < 1000; i++) {
        a[i] = a[i] & Trit::True;
    }
    for (int i = 0; i < 1000; i++) {
        REQUIRE(Trit::False == static_cast<Trit>(a[i]));
    }
    for (int i = 0; i < 1000; i++) {
        a[i] = ~a[i];
    }
    for (int i = 0; i < 1000; i++) {
        REQUIRE(Trit::True == static_cast<Trit>(a[i]));
    }
    for (int i = 0; i < 1000; i++) {
        a[i] = ~a[i];
    }
    for (int i = 0; i < 1000; i++) {
        a[i] = a[i] | Trit::Unknown;
    }
    for (int i = 0; i < 1000; i++) {
        REQUIRE(Trit::Unknown == static_cast<Trit>(a[i]));
    }
}