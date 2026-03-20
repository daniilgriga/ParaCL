#pragma once

#include "ast/stmt.hpp" // IStmtVisitor

namespace paracl::codegen
{
    class CodegenContext;

    class StmtCodegen final : public IStmtVisitor
    {
    private:
        CodegenContext& cg_;

        void visit (const ExprStmt&  node) override;
        void visit (const PrintStmt& node) override;
        void visit (const BlockStmt& node) override;
        void visit (const IfStmt&    node) override;
        void visit (const WhileStmt& node) override;

    public:
        explicit StmtCodegen (CodegenContext& cg);

        StmtCodegen (const StmtCodegen&) = delete;
        StmtCodegen& operator= (const StmtCodegen&) = delete;

        StmtCodegen (StmtCodegen&&) = delete;
        StmtCodegen& operator= (StmtCodegen&&) = delete;

        void emit (const Stmt* stmt);
    };

} // namespace paracl::codegen
