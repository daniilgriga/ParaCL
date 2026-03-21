#include "codegen/codegen_context.hpp"

#include <cassert>
#include <format>
#include <stdexcept>
#include <utility>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>

namespace paracl::codegen
{

    CodegenContext::CodegenContext (std::string module_name)
        : llvm_context_ (std::make_unique<llvm::LLVMContext>()),
          module_ (std::make_unique<llvm::Module> (std::move (module_name), *llvm_context_)),
          builder_ (std::make_unique<llvm::IRBuilder<>> (*llvm_context_))
    {
    }

    CodegenContext::~CodegenContext() = default;

    llvm::Type* CodegenContext::get_i32_type()
    {
        return llvm::Type::getInt32Ty (*llvm_context_);
    }

    llvm::Type* CodegenContext::get_void_type()
    {
        return llvm::Type::getVoidTy (*llvm_context_);
    }

    llvm::AllocaInst* CodegenContext::create_alloca_in_entry (const std::string& name)
    {
        assert (current_function_ && "create_alloca_in_entry: no active function");

        auto& entry_block = current_function_->getEntryBlock();
        llvm::IRBuilder<> tmp_builder (&entry_block, entry_block.begin());

        return tmp_builder.CreateAlloca (get_i32_type(), /*ArraySize=*/nullptr, name);
    }

    llvm::Function* CodegenContext::get_or_insert_extern_function (
        const std::string& name,
        llvm::Type* return_type,
        std::span<llvm::Type* const> param_types)
    {
        auto* fn_type = llvm::FunctionType::get (
            return_type,
            llvm::ArrayRef<llvm::Type*> (param_types.data(), param_types.size()),
            /*isVarArg=*/false);

        auto callee = module_->getOrInsertFunction (name, fn_type);
        auto* fn = llvm::cast<llvm::Function> (callee.getCallee());

        if (fn->getFunctionType() != fn_type)
        {
            throw std::logic_error (
                std::format ("get_or_insert_extern_function: '{}' already declared "
                             "with a different signature", name));
        }

        fn->setLinkage (llvm::Function::ExternalLinkage);
        return fn;
    }

    llvm::Value* CodegenContext::to_bool (llvm::Value* value)
    {
        return builder_->CreateICmpNE (
            value,
            llvm::ConstantInt::get (get_i32_type(), 0),
            "tobool");
    }

    llvm::Value* CodegenContext::to_i32_bool (llvm::Value* value)
    {
        llvm::Value* i1 = to_bool (value);
        return builder_->CreateZExt (i1, get_i32_type(), "i32bool");
    }

} // namespace paracl::codegen
