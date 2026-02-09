#include <fstream>
#include <iostream>
#include <string>

#include <FlexLexer.h>

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

int main (int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: paracl-syntax <filename>" << std::endl;
        return 1;
    }

    std::ifstream file (argv[1]);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    yyFlexLexer lexer (&file);
    yy::Driver driver (&lexer, argv[1]);

    try
    {
        driver.parse();
    }
    catch (const paracl::SyntaxError& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
