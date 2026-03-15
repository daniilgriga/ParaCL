#include "codegen/module_codegen.hpp"

#include <stdexcept>

namespace paracl::codegen
{

    ModuleCodegen::ModuleCodegen (std::string module_name)
        : cg_ (std::move (module_name))
    {
    }

    void ModuleCodegen::declare_runtime_abi()
    {
        throw std::logic_error ("ModuleCodegen::declare_runtime_abi is not implemented yet");
    }

    llvm::Function* ModuleCodegen::create_entry_function (std::string_view)
    {
        throw std::logic_error ("ModuleCodegen::create_entry_function is not implemented yet");
    }

    std::unique_ptr<llvm::Module> ModuleCodegen::lower_program (const Stmt* root,
                                                                 std::string_view)
    {
        (void) root;
        throw std::logic_error ("ModuleCodegen::lower_program is not implemented yet");
    }

} // namespace paracl::codegen
