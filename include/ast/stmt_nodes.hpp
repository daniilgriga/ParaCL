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

    class IfStmt final : public Stmt
    {
    private:
        const Expr* cond_;
        const Stmt* then_;
        const Stmt* else_;

    public:
        IfStmt (const Expr* cond, const Stmt* then_branch,
                const Stmt* else_branch = nullptr, SourceLocation loc = {})
            : Stmt (loc), cond_ (cond), then_ (then_branch), else_ (else_branch)
        {
            assert (cond_ && "IfStmt: cond must not be null");
            assert (then_ && "IfStmt: then branch must not be null");
            // else_ can be nullptr — if without else
        }

        void exec (Context& ctx) const override
        {
            if (cond_->eval (ctx) != 0)
                then_->exec (ctx);
            else if (else_)
                else_->exec (ctx);
        }
    };

    class WhileStmt final : public Stmt
    {
    private:
        const Expr* cond_;
        const Stmt* body_;

    public:
        WhileStmt (const Expr* cond, const Stmt* body, SourceLocation loc = {})
            : Stmt (loc), cond_ (cond), body_ (body)
        {
            assert (cond_ && "WhileStmt: cond must not be null");
            assert (body_ && "WhileStmt: body must not be null");
        }

        void exec (Context& ctx) const override
        {
            while (cond_->eval (ctx) != 0)
                body_->exec (ctx);
        }
    };

} // namespace paracl
