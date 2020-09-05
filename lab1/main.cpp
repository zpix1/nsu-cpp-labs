#include <iostream>
#include "TritSet.h"
using namespace std;
int main() {
    TritSet a(10);
    cout << "Capacity " << a._capacity << endl;
    for (int i = 0; i < 10; i++) {
        cout << i << " " << static_cast<int>(a.get_trit(i)) << endl;
    }
}
