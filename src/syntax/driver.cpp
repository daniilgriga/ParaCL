#include "driver.hpp"
#include "errors/errors.hpp"

void yy::Driver::error (const yy::parser::location_type& loc, const std::string& msg)
{
    const std::string file_name =
        (loc.begin.filename != nullptr) ? *loc.begin.filename : source_name_;

    const paracl::SourceLocation where
    {
        loc.begin.line,
        loc.begin.column,
        file_name
    };

    throw paracl::SyntaxError (where, msg);
}
