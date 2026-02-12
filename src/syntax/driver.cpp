#include "syntax/driver.hpp"
#include "errors/errors.hpp"

void yy::Driver::error (const yy::parser::location_type& loc, const std::string& msg)
{
    const paracl::SourceLocation where
    {
        loc.begin.line,
        loc.begin.column,
        source_name_
    };

    throw paracl::SyntaxError (where, msg);
}
