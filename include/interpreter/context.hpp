#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "errors/errors.hpp"

namespace paracl
{

    struct StringHash
    {
        using is_transparent = void;

        size_t operator() (std::string_view sv) const
        {
            return std::hash<std::string_view>{} (sv);
        }
    };

    struct StringEqual
    {
        using is_transparent = void;

        bool operator() (std::string_view a, std::string_view b) const
        {
            return a == b;
        }
    };

    class Context final
    {
    private:
        std::unordered_map<std::string, int, StringHash, StringEqual> vars_;

        std::istream& in_;
        std::ostream& out_;

    public:
        Context (std::istream& in, std::ostream& out)
            : in_ (in), out_ (out) {}

        Context() : in_ (std::cin), out_ (std::cout) {}

        bool has_var (std::string_view name) const
        {
            return vars_.find (name) != vars_.end();
        }

        int get_var (std::string_view name) const
        {
            auto it = vars_.find (name);
            if (it == vars_.end())
                throw RuntimeError ("undefined variable '" + std::string{name} + "'");

            return it->second;
        }

        int get_var (std::string_view name, SourceLocation loc) const
        {
            auto it = vars_.find (name);
            if (it == vars_.end())
                throw RuntimeError (loc, "undefined variable '" + std::string{name} + "'");

            return it->second;
        }

        void set_var (std::string_view name, int value)
        {
            vars_[std::string{name}] = value;
        }

        int read_int()
        {
            int value = 0;
            if (!(in_ >> value))
                throw RuntimeError ("failed to read integer from input");

            return value;
        }

        int read_int (SourceLocation loc)
        {
            int value = 0;
            if (!(in_ >> value))
                throw RuntimeError (loc, "failed to read integer from input");

            return value;
        }

        void print_int (int value)
        {
            out_ << value << "\n";
        }

    };

} // namespace paracl
