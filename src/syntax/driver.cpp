#include <fstream>
#include <iostream>

#include <FlexLexer.h>

#include "driver.hpp"

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
    yy::Driver driver (&lexer);
    if (!driver.parse())
    {
        std::cerr << "Parse failed" << std::endl;
        return 1;
    }

    return 0;
}
