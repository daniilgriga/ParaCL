#pragma once

#include "errors/errors.hpp"

namespace paracl
{

    class Context; // forward declaration

    class Stmt
    {
    public:
        explicit Stmt (SourceLocation loc = {}) : loc_ (loc) {}
        virtual ~Stmt() = default;

        virtual void exec (Context& ctx) const = 0;

        SourceLocation loc() const { return loc_; }

    private:
        SourceLocation loc_;
    };

} // namespace paracl
