#include "Worker.h"

#include <fstream>

void ReadfileWorker::run_operation(Context& context) {
    auto input = std::ifstream(context.arguments[0]);
    std::string str;
    while (std::getline(input, str)) {
        context.text.push_back(str);
    }
}

void ReadfileWorker::run_operation(Context& context) {

}