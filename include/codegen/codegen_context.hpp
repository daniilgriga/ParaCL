#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace paracl::codegen
{

    class CodegenContext final
    {
    private:
        std::unique_ptr<llvm::LLVMContext> llvm_context_;
        std::unique_ptr<llvm::Module> module_;
        std::unique_ptr<llvm::IRBuilder<>> builder_;

        llvm::Function* current_function_ = nullptr;
        std::unordered_map<std::string, llvm::AllocaInst*> named_values_;

    public:
        explicit CodegenContext (std::string module_name);
        ~CodegenContext();

        CodegenContext (const CodegenContext&) = delete;
        CodegenContext& operator= (const CodegenContext&) = delete;

        CodegenContext (CodegenContext&&) = delete;
        CodegenContext& operator= (CodegenContext&&) = delete;

        llvm::LLVMContext& llvm_context() { return *llvm_context_; }
        const llvm::LLVMContext& llvm_context() const { return *llvm_context_; }

        llvm::Module& module() { return *module_; }
        const llvm::Module& module() const { return *module_; }

        llvm::IRBuilder<>& builder() { return *builder_; }
        const llvm::IRBuilder<>& builder() const { return *builder_; }

        llvm::Function* current_function() const { return current_function_; }
        void set_current_function (llvm::Function* fn) { current_function_ = fn; }

        std::unordered_map<std::string, llvm::AllocaInst*>& named_values()
        {
            return named_values_;
        }

        const std::unordered_map<std::string, llvm::AllocaInst*>& named_values() const
        {
            return named_values_;
        }
    };

} // namespace paracl::codegen
