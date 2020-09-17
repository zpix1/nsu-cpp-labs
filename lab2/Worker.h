#ifndef LAB2_WORKER_H
#define LAB2_WORKER_H

#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "WorkflowExceptions.h"

namespace Workflow {
    using TextContainer = std::vector<std::string>;
    using ArgumentList = std::vector<std::string>;
    using WorkerID = int;

    enum class InputOutputMode {
        FileMode,
        CmdlineFlagMode,
        None
    };

    struct Context {
        ArgumentList arguments;
        TextContainer text;
    };

    class Worker {
    public:
        virtual void run_operation(Context &context) = 0;
        virtual bool check_arguments(const ArgumentList& arguments) = 0;
    };

    struct Scheme {
        std::unordered_map<WorkerID, std::pair<std::unique_ptr<Worker>, ArgumentList> > id2worker;
        std::vector<WorkerID> execution_flow;
    };

    class Parser {
        [[nodiscard]] virtual Scheme parse(const TextContainer &text) const = 0;
    };

    class Validator {
        virtual bool validate(const Scheme &scheme, const InputOutputMode mode) const = 0;

        [[nodiscard]] virtual InputOutputMode check_mode(const Scheme &scheme) const = 0;
    };

    class Executor {
        // FileMode
        virtual void execute(const Scheme &scheme, std::ifstream input, std::ofstream output) = 0;

        // CmdlineFlagMode
        virtual void execute(const Scheme &scheme) = 0;
    };

    class WorkflowExecutor : public Parser, public Validator, public Executor {
    public:
        [[nodiscard]] Scheme parse(const TextContainer &text) const override;

        bool validate(const Scheme &scheme, const InputOutputMode mode) const override;

        InputOutputMode check_mode(const Scheme &scheme) const override;

        void execute(const Scheme &scheme, std::ifstream input, std::ofstream output) override;

        void execute(const Scheme &scheme) override;
    };

    class ReadfileWorker : public Worker {
    public:
        void run_operation(Context &context) override;
        inline bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 1;
        };
    };

    class WritefileWorker : public Worker {
    public:
        void run_operation(Context &context) override;
        inline bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 1;
        };
    };

    class GrepWorker : public Worker {
    public:
        void run_operation(Context &context) override;
        inline bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 1;
        };
    };

    class SortWorker : public Worker {
    public:
        void run_operation(Context &context) override;
        inline bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 0;
        };
    };

    class ReplaceWorker : public Worker {
    public:
        void run_operation(Context &context) override;
        inline bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 2;
        };
    };

    class DumpWorker : public Worker {
    public:
        void run_operation(Context &context) override;
        inline bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 1;
        };
    };
}

#endif //LAB2_WORKER_H
