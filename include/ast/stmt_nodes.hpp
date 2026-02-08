#pragma once

#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include "ast/stmt.hpp"
#include "ast/expr.hpp"
#include "interpreter/context.hpp"

namespace paracl
{

    class AssignStmt final : public Stmt
    {
    private:
        std::string name_;
        const Expr* rhs_;

    public:
        AssignStmt (std::string name, const Expr* rhs, SourceLocation loc = {})
            : Stmt (loc), name_ (std::move (name)), rhs_ (rhs)
        {
            assert (rhs_ && "AssignStmt: rhs must not be null");
        }

        void exec (Context& ctx) const override
        {
            ctx.set_var (name_, rhs_->eval (ctx));
        }
    };

    class PrintStmt final : public Stmt
    {
    private:
        const Expr* expr_;

    public:
        explicit PrintStmt (const Expr* expr, SourceLocation loc = {})
            : Stmt (loc), expr_ (expr)
        {
            assert (expr_ && "PrintStmt: expr must not be null");
        }

        void exec (Context& ctx) const override
        {
            ctx.print_int (expr_->eval (ctx));
        }
    };

    class ExprStmt final : public Stmt // for funcs or increment in future
    {
    private:
        const Expr* expr_;

    public:
        explicit ExprStmt (const Expr* expr, SourceLocation loc = {})
            : Stmt (loc), expr_ (expr)
        {
            assert (expr_ && "ExprStmt: expr must not be null");
        }

        void exec (Context& ctx) const override
        {
            expr_->eval (ctx);
        }
    };

    class BlockStmt final : public Stmt
    {
    private:
        std::vector<const Stmt*> stmts_;

    public:
        explicit BlockStmt (std::vector<const Stmt*> stmts, SourceLocation loc = {})
            : Stmt (loc), stmts_ (std::move (stmts))
        {
            for (const auto* s : stmts_)
            {
                assert (s && "BlockStmt: statement must not be null");
            }
        }

        void exec (Context& ctx) const override
        {
            for (const auto* stmt : stmts_)
            {
                stmt->exec (ctx);
            }
        }
    };

} // namespace paracl
