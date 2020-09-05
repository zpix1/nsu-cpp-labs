#include <iostream>
#include <map>
#include <cassert>
#include "TritSet.h"

using namespace std;

void log(TritSet &a) {
    cout << "Cap: " << a.capacity() << " Len: " << a.length() << " 9: " << static_cast<int>(a.get_trit(9)) << endl;
}

int main() {
    TritSet set(1000);
    // length of internal array
    size_t allocLength = set.capacity();
    assert(allocLength >= 1000 * 2 / 8 / sizeof(unsigned int));
    // 1000*2 - min bits count
    // 1000*2 / 8 - min bytes count
    // 1000*2 / 8 / sizeof(uint) - min uint[] size

    //не выделяет никакой памяти
//    set[1000’000’000] = Unknown;
    set.set_trit(1000000000, Trit::Unknown);
    assert(allocLength == set.capacity());

    // false, but no exception or memory allocation
//    if (set[2000’000’0`00]==True){}
    if (set.get_trit(2000000000) == Trit::True) {}
    assert(allocLength == set.capacity());

    //выделение памяти
    set.set_trit(1000000000, Trit::True);
    assert(allocLength < set.capacity());

    //no memory operations
    allocLength = set.capacity();
    set.set_trit(1000000000, Trit::Unknown);
    set.set_trit(1000000, Trit::False);
    assert(allocLength == set.capacity());

    //освобождение памяти до начального значения или
    //до значения необходимого для хранения последнего установленного трита
    //в данном случае для трита 1000’000
    set.shrink();
    assert(allocLength > set.capacity());

    TritSet a(10);
    log(a);
    a.set_trit(9, Trit::True);
    log(a);
    a.set_trit(100, Trit::True);
    log(a);
    a.set_trit(100, Trit::Unknown);
    log(a);
    a.shrink();
    log(a);
}
