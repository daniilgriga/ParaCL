#pragma once

#include <cassert>
#include <string>
#include <utility>

#include "ast/expr.hpp"
#include "ast/operators.hpp"
#include "interpreter/context.hpp"

namespace paracl
{

    class IntLiteral final : public Expr
    {
    private:
        int value_;

    public:
        explicit IntLiteral (int value, SourceLocation loc = {})
            : Expr (loc), value_ (value) {}

        int eval (Context&) const override
        {
            return value_;
        }
    };

    class VarRef final : public Expr
    {
    private:
        std::string name_;

    public:
        explicit VarRef (std::string name, SourceLocation loc = {})
            : Expr (loc), name_ (std::move (name)) {}

        int eval (Context& ctx) const override
        {
            return ctx.get_var (name_, loc());
        }
    };

    class ReadExpr final : public Expr
    {
    public:
        explicit ReadExpr (SourceLocation loc = {})
            : Expr (loc) {}

        int eval (Context& ctx) const override
        {
            return ctx.read_int (loc());
        }
    };

    class UnaryExpr final : public Expr
    {
    private:
        UnOp op_;
        const Expr* child_;

    public:
        UnaryExpr (UnOp op, const Expr* child, SourceLocation loc = {})
            : Expr (loc), op_ (op), child_ (child)
        {
            assert (child_ && "UnaryExpr: child must not be null");
        }

        int eval (Context& ctx) const override
        {
            int val = child_->eval (ctx);

            switch (op_)
            {
                case UnOp::Neg:
                    return -val;
                case UnOp::Not:
                    return val == 0 ? 1 : 0;
            }

            throw RuntimeError (loc(), "unknown unary operator"); // dead code for compiler
        }
    };

    class BinaryExpr final : public Expr
    {
    private:
        BinOp op_;
        const Expr* lhs_;
        const Expr* rhs_;

    public:
        BinaryExpr (BinOp op, const Expr* lhs, const Expr* rhs,
                    SourceLocation loc = {})
            : Expr (loc), op_ (op), lhs_ (lhs), rhs_ (rhs)
        {
            assert (lhs_ && "BinaryExpr: lhs must not be null");
            assert (rhs_ && "BinaryExpr: rhs must not be null");
        }

        int eval (Context& ctx) const override
        {
            int left = lhs_->eval (ctx);
            int right = rhs_->eval (ctx);

            switch (op_)
            {
                case BinOp::Add: return left + right;
                case BinOp::Sub: return left - right;
                case BinOp::Mul: return left * right;
                case BinOp::Div:
                    if (right == 0)
                        throw RuntimeError (loc(), "division by zero");
                    return left / right;
                case BinOp::Mod:
                    if (right == 0)
                        throw RuntimeError (loc(), "division by zero");
                    return left % right;
                case BinOp::Lt:  return left < right  ? 1 : 0;
                case BinOp::Le:  return left <= right ? 1 : 0;
                case BinOp::Gt:  return left > right  ? 1 : 0;
                case BinOp::Ge:  return left >= right ? 1 : 0;
                case BinOp::Eq:  return left == right ? 1 : 0;
                case BinOp::Ne:  return left != right ? 1 : 0;
                case BinOp::And: return left != 0 && right != 0 ? 1 : 0;
                case BinOp::Or:  return left != 0 || right != 0 ? 1 : 0;
                case BinOp::Xor: return (left != 0) != (right != 0) ? 1 : 0;
            }

            throw RuntimeError (loc(), "unknown binary operator"); // dead code for compiler
        }
    };

} // namespace paracl
