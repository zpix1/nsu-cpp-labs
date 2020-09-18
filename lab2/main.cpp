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
    bool is_output_provided = app.count("-o");
    try {
        if (is_input_provided) Workflow::file_should_exist(inputfilename);

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
            std::cout << "I mode" << std::endl;
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