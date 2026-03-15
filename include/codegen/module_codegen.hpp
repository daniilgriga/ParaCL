#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "codegen/codegen_context.hpp"

namespace llvm
{
    class Function;
    class Module;
}

namespace paracl
{
    class Stmt;
}

namespace paracl::codegen
{
    class ModuleCodegen final
    {
    public:
        explicit ModuleCodegen (std::string module_name);

        ModuleCodegen (const ModuleCodegen&) = delete;
        ModuleCodegen& operator= (const ModuleCodegen&) = delete;

        ModuleCodegen (ModuleCodegen&&) = delete;
        ModuleCodegen& operator= (ModuleCodegen&&) = delete;

        std::unique_ptr<llvm::Module> lower_program (const Stmt* root,
                                                     std::string_view entry_name = "__pcl_start");

    private:
        void declare_runtime_abi();
        llvm::Function* create_entry_function (std::string_view entry_name);

        CodegenContext cg_;
    };

} // namespace paracl::codegen
