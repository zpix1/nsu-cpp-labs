#ifndef LAB2_WORKER_H
#define LAB2_WORKER_H

#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "WorkflowExceptions.h"

using TextContainer = std::vector<std::string>;
using ArgumentList = std::vector<std::string>;
using WorkerID = int;

enum class Mode {
    FileMode,
    CmdlineFlagMode
};

struct Context {
    ArgumentList arguments;
    TextContainer text;
};

class Worker {
public:
    virtual void run_operation(Context& context) = 0;
};

struct Scheme {
    std::unordered_map< WorkerID, std::pair<Worker, ArgumentList> > id2worker;
    std::vector<WorkerID> execution_flow;
};

class Parser {
    virtual Scheme parse(const TextContainer& text) const = 0;
};

class Validator {
    virtual void validate(const Scheme& scheme) const = 0;
    virtual Mode check_mode(const Scheme& scheme) const = 0;
};

class Executor {
    // FileMode
    virtual void execute(const Scheme& scheme, std::ifstream input, std::ofstream output);
    // CmdlineFlagMode
    virtual void execute(const Scheme& scheme);
};

class WorkflowExecutor: public Parser, public Validator, public Executor {
    Scheme parse(const TextContainer& text) const override;
    void validate(const Scheme& scheme) const override;
    Mode check_mode(const Scheme& scheme) const override;
    void execute(const Scheme& scheme, std::ifstream input, std::ofstream output) override;
    void execute(const Scheme& scheme) override;
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
