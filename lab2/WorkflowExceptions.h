#include <exception>
#include <string>
#include <utility>

namespace Workflow {
    class ParsingException : public std::exception {
        std::string error_message;
        std::size_t line_idx;
    public:
        explicit ParsingException(std::string msg, const std::size_t nline) :
                error_message(std::move(msg)),
                line_idx(nline) {};

        [[nodiscard]] inline const char *what() const noexcept override {
            return error_message.c_str();
        }

        inline std::size_t get_line_idx() const {
            return line_idx;
        }
    };

    class ValidationException : public std::exception {
        std::string error_message;
        std::size_t element_idx;
    public:
        enum class ErrorDomain {
            ExecutionFlow,
            Description
        };
    private:
        ErrorDomain domain;
    public:
        ValidationException(std::string msg, const ErrorDomain domain, const std::size_t element_idx) :
                error_message(std::move(msg)),
                domain(domain),
                element_idx(element_idx) {};

        [[nodiscard]] inline const char *what() const noexcept override {
            return error_message.c_str();
        }

        [[nodiscard]] inline auto get_domain() const {
            return domain;
        }

        inline auto get_element_idx() const {
            return element_idx;
        }
    };
}