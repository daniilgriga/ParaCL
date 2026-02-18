#include <fstream>
#include <iostream>
#include <string>

#include "errors/errors.hpp"
#include "interpreter/context.hpp"
#include "syntax/driver.hpp"
#include "syntax/lexer.hpp"

int main (int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: paracl <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    std::ifstream file (filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
        return 1;
    }

    yy::Lexer lexer (&file);
    yy::Driver driver (&lexer, filename);

    try
    {
        driver.parse();

        paracl::Context ctx (std::cin, std::cout);
        driver.root()->exec (ctx);
    }
    catch (const paracl::ParaCLError& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
