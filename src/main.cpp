#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

#include "errors/errors.hpp"
#include "interpreter/context.hpp"
#include "syntax/driver.hpp"
#include "syntax/lexer.hpp"

#ifdef PARACL_CODEGEN
#include "codegen/module_codegen.hpp"
#include <llvm/Support/raw_ostream.h>
#endif

#if !defined(PARACL_INTERP) && !defined(PARACL_CODEGEN)
#error "Define either PARACL_INTERP or PARACL_CODEGEN at compile time"
#endif

#ifdef PARACL_CODEGEN
namespace
{

    std::string replace_extension (const std::string& path, const std::string& new_ext)
    {
        const auto dot = path.rfind ('.');
        const auto sep = path.find_last_of ("/\\");

        if (dot != std::string::npos &&
           (sep == std::string::npos || dot > sep))
        {
            return path.substr (0, dot) + new_ext;
        }

        return path + new_ext;
    }
} // namespace
#endif

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

#ifdef PARACL_INTERP
        paracl::Context ctx (std::cin, std::cout);
        driver.root()->exec (ctx);

#elif defined(PARACL_CODEGEN)
        paracl::codegen::ModuleCodegen codegen (filename);
        auto module = codegen.lower_program (driver.root());

        const std::string output_filename = replace_extension (filename, ".ll");

        std::error_code ec;
        llvm::raw_fd_ostream out (output_filename, ec);

        if (ec)
            throw std::runtime_error ("failed to open output file '" +
                                      output_filename + "': " + ec.message());

        module->print (out, nullptr);
#endif
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
