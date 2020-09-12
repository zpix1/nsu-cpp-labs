//
// Created by John on 12.09.2020.
//

#ifndef LAB2_WORKER_H
#define LAB2_WORKER_H

#include <string>
#include <utility>
#include <vector>
#include <memory>

class Worker {
public:
    Worker(std::string argument, std::shared_ptr<std::vector<std::string> > text) : argument(std::move(argument)),
                                                                                    text(std::move(text)) {}

    virtual void execute() = 0;
    inline auto get_text() {
        return text;
    }
protected:
    std::string argument;
    std::shared_ptr<std::vector<std::string> > text;
};

class ReadfileWorker : Worker {
public:
    using Worker::Worker;
    using Worker::get_text;
    void execute() override;
};

#endif //LAB2_WORKER_H
