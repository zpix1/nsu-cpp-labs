#include <iostream>
#include <memory>

#include "thirdparty/CLI11.hpp"

#include "utility.h"
#include "Worker.h"

int main(int argc, char **argv) {
    CLI::App app{"Workflow Executor"};

    std::string input_filename, output_filename, instructions_filename;
    app.add_option("-i, --input", input_filename, "Input file");
    app.add_option("-o, --output", output_filename, "Output file");
    app.add_option("instructions", instructions_filename, "Workflow instructions file")->required();

    CLI11_PARSE(app, argc, argv)

    bool is_input_provided = app.count("-i");
    bool is_output_provided = app.count("-o");
    try {
        if (is_input_provided) Workflow::file_should_exist(input_filename);

        Workflow::TextContainer instructions;
        Workflow::readfile(instructions_filename, instructions);
        Workflow::WorkflowExecutor worker;

        auto scheme = worker.parse(instructions);

        if (is_input_provided && is_output_provided) {
            worker.validate(scheme, Workflow::InputOutputMode::FlagIO);
            std::ifstream input(input_filename);
            std::ofstream output(output_filename);
            worker.execute(scheme, input, output);
        } else if (is_input_provided) {
            std::cout << "I mode" << std::endl;
            worker.validate(scheme, Workflow::InputOutputMode::FlagI);
            std::ifstream input(input_filename);
            worker.execute(scheme, input);
        } else if (is_output_provided) {
            worker.validate(scheme, Workflow::InputOutputMode::FlagI);
            std::ofstream output(output_filename);
            worker.execute(scheme, output);
        } else {
            worker.validate(scheme, Workflow::InputOutputMode::FlagZero);
            worker.execute(scheme);
        }
    } catch (const Workflow::ParsingException& e) {
        std::cerr << "WorkFlow Executor run failed" << std::endl;
        std::cerr << "Parse error: At line " << std::to_string(e.get_line_idx()) << std::endl;
        std::cerr << "Parse error: " << e.what() << std::endl;
        return 1;
    } catch (const Workflow::ValidationException& e) {
        std::cerr << "WorkFlow Executor run failed" << std::endl;
        if (e.get_domain() == Workflow::ValidationException::ErrorDomain::ExecutionFlow) {
            std::cerr << "Validation error: in the flow line at the element " << std::to_string(e.get_element_idx())
                      << std::endl;
        } else if (e.get_domain() == Workflow::ValidationException::ErrorDomain::Description) {
            std::cerr << "Validation error: in the description at the element " << std::to_string(e.get_element_idx())
                      << std::endl;
        }
        std::cerr << "Validation error: " << e.what() << std::endl;
        return 1;
    } catch (const Workflow::FileException& e) {
        std::cerr << "WorkFlow Executor run failed" << std::endl;
        std::cerr << "File error: " << e.what() << std::endl;
        std::cerr << "File error: " << e.get_filename() << std::endl;
        return 1;
    }
    return 0;
}