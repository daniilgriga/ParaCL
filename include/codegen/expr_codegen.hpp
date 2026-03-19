#pragma once

#include "ast/expr.hpp" // IExprVisitor

namespace llvm
{
    class Value;
    class AllocaInst;
}

namespace paracl::codegen
{
    class CodegenContext;

    class ExprCodegen final : public IExprVisitor
    {
    private:
        CodegenContext& cg_;
        llvm::Value* result_ = nullptr;

        void visit (const IntLiteral&  node) override;
        void visit (const VarRef&      node) override;
        void visit (const ReadExpr&    node) override;
        void visit (const AssignExpr&  node) override;
        void visit (const UnaryExpr&   node) override;
        void visit (const BinaryExpr&  node) override;

        llvm::AllocaInst* get_or_create_slot (const std::string& name);

    public:
        explicit ExprCodegen (CodegenContext& cg);

        ExprCodegen (const ExprCodegen&) = delete;
        ExprCodegen& operator= (const ExprCodegen&) = delete;

        ExprCodegen (ExprCodegen&&) = delete;
        ExprCodegen& operator= (ExprCodegen&&) = delete;

        llvm::Value* emit (const Expr* expr);
    };

} // namespace paracl::codegen
