#include <exception>
#include <string>

namespace Workflow {
    class ParsingException: public std::exception {
        std::string error_message;
        std::size_t nline;
    public:
        explicit ParsingException(const std::string& msg, const std::size_t nline) :
         error_message(msg),
         nline(nline) 
         {};

        inline const char* what() const throw () {
            return error_message.c_str();
        }
        inline const std::size_t get_line_idx() {
            return nline;
        }
    };
}