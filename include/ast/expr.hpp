#pragma once

#include "errors/errors.hpp"

namespace paracl
{

    class Context; // forward declaration

    class Expr
    {
    public:
        explicit Expr (SourceLocation loc = {}) : loc_ (loc) {}
        virtual ~Expr() = default;

        virtual int eval (Context& ctx) const = 0;

        SourceLocation loc() const { return loc_; }

    private:
        SourceLocation loc_;
    };

} // namespace paracl
