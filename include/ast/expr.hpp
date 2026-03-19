#pragma once

#include "errors/errors.hpp"

namespace paracl
{

    class Context; // forward declaration

    class IntLiteral;
    class VarRef;
    class ReadExpr;
    class AssignExpr;
    class UnaryExpr;
    class BinaryExpr;

    class IExprVisitor
    {
    public:
        virtual ~IExprVisitor() = default;

        virtual void visit (const IntLiteral&  node) = 0;
        virtual void visit (const VarRef&      node) = 0;
        virtual void visit (const ReadExpr&    node) = 0;
        virtual void visit (const AssignExpr&  node) = 0;
        virtual void visit (const UnaryExpr&   node) = 0;
        virtual void visit (const BinaryExpr&  node) = 0;
    };

    class Expr
    {
    private:
        SourceLocation loc_;

    public:
        explicit Expr (SourceLocation loc = {}) : loc_ (loc) {}
        virtual ~Expr() = default;

        virtual int eval (Context& ctx) const = 0;
        virtual void accept (IExprVisitor& v) const = 0;

        SourceLocation loc() const { return loc_; }
    };

} // namespace paracl
