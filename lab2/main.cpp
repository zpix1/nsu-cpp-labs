#include <iostream>
#include <memory>
#include <iostream>

#include "thirdparty/CLI11.hpp"

#include "utility.h"
#include "Worker.h"

int main(int argc, char **argv) {
    CLI::App app{"Workflow Executor"};

    std::string inputfilename, outputfilename, instructionsfilename;
    app.add_option("-i, --input", inputfilename, "Input file");
    app.add_option("-o, --output", outputfilename, "Output file");
    app.add_option("instructions", instructionsfilename, "Workflow instructions file")->required();

    CLI11_PARSE(app, argc, argv);

    bool is_input_provided = app.count("-i");
    bool is_output_provided = app.count("-i");

    try {
        Workflow::TextContainer instructions;
        Workflow::readfile(instructionsfilename, instructions);
        Workflow::WorkflowExecutor worker;

        auto scheme = worker.parse(instructions);

        if (is_input_provided && is_output_provided) {
            worker.validate(scheme, Workflow::InputOutputMode::FlagIO);
            std::ifstream input(inputfilename);
            std::ofstream output(outputfilename);
            worker.execute(scheme, input, output);
        } else if (is_input_provided) {
            worker.validate(scheme, Workflow::InputOutputMode::FlagI);
            std::ifstream input(inputfilename);
            worker.execute(scheme, input);
        } else if (is_output_provided) {
            worker.validate(scheme, Workflow::InputOutputMode::FlagI);
            std::ofstream output(outputfilename);
            worker.execute(scheme, output);
        } else {
            worker.validate(scheme, Workflow::InputOutputMode::FlagZero);
            worker.execute(scheme);
        }
    } catch (const Workflow::ParsingException& e) {
        std::cerr << "WorkFlow Executor run failed" << std::endl;
        std::cerr << "Parsing error: At line " << std::to_string(e.get_line_idx()) << std::endl;
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return 1;
    } catch (const Workflow::ValidationException& e) {
        std::cerr << "WorkFlow Executor run failed" << std::endl;
        if (e.get_domain() == Workflow::ValidationException::ErrorDomain::ExecutionFlow) {
            std::cerr << "Validating error: in the flow line at the element " << std::to_string(e.get_element_idx()) << std::endl;
        } else if (e.get_domain() == Workflow::ValidationException::ErrorDomain::Description) {
            std::cerr << "Validating error: in the description at the element " << std::to_string(e.get_element_idx()) << std::endl;
        }
        std::cerr << "Validating error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}