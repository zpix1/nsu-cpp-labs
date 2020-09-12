#include <iostream>
#include <memory>
#include <iostream>

#include "Worker.h"
using namespace std;

int main() {
    ReadfileWorker v("test.txt", nullptr);

    v.execute();

    cout << (*v.get_text())[0] << endl;
}
