#ifndef LAB2_WORKER_H
#define LAB2_WORKER_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <memory>

using TextContainer = std::vector<std::string>;
using ArgumentList = std::vector<std::string>;
using Scheme = std::vector<Worker>;

class Parser {
    virtual Scheme parse(TextContainer text) = 0;
}

class Validator {
    virtual void validate(Scheme scheme) = 0;
};

class Execute {
    virtual std::ofstream execute(std::ifstream input);
};

struct Context {
    ArgumentList arguments;
    TextContainer text;
};

class Worker {
public:
    virtual void run_operation(Context& context) = 0;
};

class ReadfileWorker : public Worker {
public:
    void run_operation(Context& context) override;
};

class WritefileWorker : public Worker {
public:
    void run_operation(Context& context) override;
};

class GrepWorker : public Worker {
public:
    void run_operation(Context& context) override;
};

class SortWorker : public Worker {
public:
    void run_operation(Context& context) override;
};

class ReplaceWorker : public Worker {
public:
    void run_operation(Context& context) override;
};

class DumpWorker : public Worker {
public:
    void run_operation(Context& context) override;
};

#endif //LAB2_WORKER_H
