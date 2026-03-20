#pragma once

#include "errors/errors.hpp"

namespace paracl
{

    class Context; // forward declaration

    class ExprStmt;
    class PrintStmt;
    class BlockStmt;
    class IfStmt;
    class WhileStmt;

    class IStmtVisitor
    {
    public:
        virtual ~IStmtVisitor() = default;

        virtual void visit (const ExprStmt&  node) = 0;
        virtual void visit (const PrintStmt& node) = 0;
        virtual void visit (const BlockStmt& node) = 0;
        virtual void visit (const IfStmt&    node) = 0;
        virtual void visit (const WhileStmt& node) = 0;
    };

    class Stmt
    {
    private:
        SourceLocation loc_;

    public:
        explicit Stmt (SourceLocation loc = {}) : loc_ (loc) {}
        virtual ~Stmt() = default;

        virtual void exec (Context& ctx) const = 0;
        virtual void accept (IStmtVisitor& v) const = 0;

        SourceLocation loc() const { return loc_; }
    };

} // namespace paracl
