#pragma once

namespace llvm
{
    class Value;
}

namespace paracl
{
    class Expr;
}

namespace paracl::codegen
{
    class CodegenContext;

    class ExprCodegen final
    {
    public:
        explicit ExprCodegen (CodegenContext& cg);

        ExprCodegen (const ExprCodegen&) = delete;
        ExprCodegen& operator= (const ExprCodegen&) = delete;

        ExprCodegen (ExprCodegen&&) = delete;
        ExprCodegen& operator= (ExprCodegen&&) = delete;

        llvm::Value* emit (const Expr* expr);

    private:
        CodegenContext& cg_;
    };

} // namespace paracl::codegen
