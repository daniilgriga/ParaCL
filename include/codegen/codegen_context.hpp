#pragma once

#include <memory>
#include <span>
#include <string>
#include <unordered_map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

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

        std::unique_ptr<llvm::Module> module_owner() { return std::move (module_); }

        llvm::Type* get_i32_type();

        llvm::Type* get_void_type();

        llvm::AllocaInst* create_alloca_in_entry (const std::string& name);

        llvm::Function* get_or_insert_extern_function (
            const std::string& name,
            llvm::Type* return_type,
            std::span<llvm::Type* const> param_types);

        llvm::Value* to_bool (llvm::Value* value);

        llvm::Value* to_i32_bool (llvm::Value* value);
    };

} // namespace paracl::codegen
