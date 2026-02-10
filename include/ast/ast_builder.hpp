#pragma once

#include <vector>
#include <memory>
#include <type_traits>

#include "ast/expr.hpp"
#include "ast/stmt.hpp"

namespace paracl
{

    class AstBuilder
    {
    private:
        std::vector<std::unique_ptr<Expr>> exprs_;
        std::vector<std::unique_ptr<Stmt>> stmts_;

        const Stmt* root_ = nullptr;

    public:
        template <typename T, typename... Args>
        const T* make_expr (Args&&... args)
        {
            static_assert (std::is_base_of_v<Expr, T>, "make_expr: T must derive from Expr");

            auto ptr = std::make_unique<T> (std::forward<Args> (args)...);
            const T* raw = ptr.get();
            exprs_.push_back (std::move (ptr));

            return raw;
        }

        template <typename T, typename... Args>
        const T* make_stmt (Args&&... args)
        {
            static_assert (std::is_base_of_v<Stmt, T>, "make_stmt: T must derive from Stmt");

            auto ptr = std::make_unique<T> (std::forward<Args> (args)...);
            const T* raw = ptr.get();
            stmts_.push_back (std::move (ptr));

            return raw;
        }

        void set_root (const Stmt* root) { root_ = root; }

        const Stmt* root() const { return root_; }
    };

} // namespace paracl
