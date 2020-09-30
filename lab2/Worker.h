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
        // Use input/output from instructions
        FlagZero,
        // Use output from instructions and input from stream
        FlagI,
        // Use input from instructions and output from stream
        FlagO,
        // Use input and output from streams
        FlagIO,
        None
    };

    struct Context {
        TextContainer text;
    };

    class Worker {
    public:
        virtual void run_operation(const ArgumentList& arguments, Context& context) = 0;

        virtual bool check_arguments(const ArgumentList& arguments) = 0;
    };

    struct Scheme {
        std::unordered_map<WorkerID, std::pair<std::unique_ptr<Worker>, ArgumentList> > id2worker;
        std::vector<WorkerID> execution_flow;
    };

    class Parser {
        [[nodiscard]] virtual Scheme parse(const TextContainer& text) const = 0;
    };

    class Validator {
        virtual void validate(const Scheme& scheme, InputOutputMode mode) const = 0;
    };

    class Executor {
        virtual void execute_with_ctx(const Scheme& scheme, Context& ctx) = 0;

        // FlagZero
        virtual void execute(const Scheme& scheme) = 0;

        // FlagI
        virtual void execute(const Scheme& scheme, std::istream& input) = 0;

        // FlagO
        virtual void execute(const Scheme& scheme, std::ostream& output) = 0;

        // FlagIO
        virtual void execute(const Scheme& scheme, std::istream& input, std::ostream& output) = 0;
    };

    class WorkflowExecutor : public Parser, public Validator, public Executor {
        void execute_with_ctx(const Scheme& scheme, Context& ctx) override;

    public:
        [[nodiscard]] Scheme parse(const TextContainer& text) const override;

        void validate(const Scheme& scheme, InputOutputMode mode) const override;

        void execute(const Scheme& scheme) override;

        // FlagI
        void execute(const Scheme& scheme, std::istream& input) override;

        // FlagO
        void execute(const Scheme& scheme, std::ostream& output) override;

        // FlagIO
        void execute(const Scheme& scheme, std::istream& input, std::ostream& output) override;
    };

    class ReadfileWorker : public Worker {
    public:
        void run_operation(const ArgumentList& arguments, Context& context) override;

        bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 1;
        };
    };

    class WritefileWorker : public Worker {
    public:
        void run_operation(const ArgumentList& arguments, Context& context) override;

        bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 1;
        };
    };

    class GrepWorker : public Worker {
    public:
        void run_operation(const ArgumentList& arguments, Context& context) override;

        bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 1;
        };
    };

    class SortWorker : public Worker {
    public:
        void run_operation(const ArgumentList& arguments, Context& context) override;

        bool check_arguments(const ArgumentList& arguments) override {
            return arguments.empty();
        };
    };

    class ReplaceWorker : public Worker {
    public:
        void run_operation(const ArgumentList& arguments, Context& context) override;

        bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 2;
        };
    };

    class DumpWorker : public Worker {
    public:
        void run_operation(const ArgumentList& arguments, Context& context) override;

        bool check_arguments(const ArgumentList& arguments) override {
            return arguments.size() == 1;
        };
    };
}

#endif //LAB2_WORKER_H
