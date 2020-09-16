#ifndef LAB2_WORKER_H
#define LAB2_WORKER_H

#include <string>
#include <utility>
#include <vector>
#include <memory>

class Validator {
    
}

struct Context {
    
}

class Worker {
public:
    virtual void execute() = 0;
};

class ReadfileWorker : public Worker {
public:
    void execute() override;
};

#endif //LAB2_WORKER_H
