#include "codegen/module_codegen.hpp"

#include <array>
#include <stdexcept>
#include <string>

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include "ast/stmt.hpp"
#include "codegen/stmt_codegen.hpp"

namespace paracl::codegen
{

    ModuleCodegen::ModuleCodegen (std::string module_name)
        : cg_ (std::move (module_name))
    {
    }

    void ModuleCodegen::declare_runtime_abi()
    {
        // void __pcl_print(i32)
        std::array<llvm::Type*, 1> print_params = { cg_.get_i32_type() };
        cg_.get_or_insert_extern_function ("__pcl_print", cg_.get_void_type(), print_params);

        // i32 __pcl_scan()
        cg_.get_or_insert_extern_function ("__pcl_scan", cg_.get_i32_type(), {});
    }

    llvm::Function* ModuleCodegen::create_entry_function (std::string_view entry_name)
    {
        // __pcl_start
        auto* fn_type = llvm::FunctionType::get (cg_.get_void_type(), /*isVarArg=*/false);

        auto* fn = llvm::Function::Create (
            fn_type,
            llvm::Function::ExternalLinkage,
            llvm::Twine (entry_name),
            cg_.module());

        auto* entry_bb = llvm::BasicBlock::Create (cg_.llvm_context(), "entry", fn);
        cg_.builder().SetInsertPoint (entry_bb);
        cg_.set_current_function (fn);

        return fn;
    }

    std::unique_ptr<llvm::Module> ModuleCodegen::lower_program (const Stmt* root,
                                                                 std::string_view entry_name)
    {
        declare_runtime_abi();

        auto* entry_fn = create_entry_function (entry_name);

        if (root)
        {
            StmtCodegen stmt_cg { cg_ };
            stmt_cg.emit (root);
        }

        cg_.builder().CreateRetVoid();

        std::string errors;
        llvm::raw_string_ostream err_stream { errors };

        if (llvm::verifyFunction (*entry_fn, &err_stream))
        {
            throw std::logic_error ("IR verification failed for '" +
                                    std::string (entry_name) + "': " + errors);
        }

        if (llvm::verifyModule (cg_.module(), &err_stream))
        {
            throw std::logic_error ("IR module verification failed: " + errors);
        }

        return cg_.module_owner();
    }

} // namespace paracl::codegen
