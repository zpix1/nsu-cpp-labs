#include <exception>
#include <string>

namespace Workflow {
    class ParsingException: public std::exception {
        std::string error_message;
        std::size_t line_idx;
    public:
        explicit ParsingException(const std::string& msg, const std::size_t nline) :
                error_message(msg),
                line_idx(nline)
         {};

        inline const char* what() const throw () {
            return error_message.c_str();
        }
        inline std::size_t get_line_idx() const {
            return line_idx;
        }
    };
    class ValidationException: public std::exception {
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
        ValidationException(const std::string& msg, const ErrorDomain domain, const std::size_t element_idx) :
                error_message(msg),
                domain(domain),
                element_idx(element_idx)
        {};

        inline const char* what() const throw () {
            return error_message.c_str();
        }
        inline auto get_domain() const {
            return domain;
        }
        inline auto get_element_idx() const {
            return element_idx;
        }
    };
}