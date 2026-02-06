#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "errors/errors.hpp"

namespace paracl
{

    class Context
    {
    private:
        std::unordered_map<std::string, int> vars_;

        std::istream& in_;
        std::ostream& out_;

    public:
        Context (std::istream& in, std::ostream& out)
            : in_ (in), out_ (out) {}

        Context() : in_ (std::cin), out_ (std::cout) {}

        bool has_var (const std::string& name) const
        {
            return vars_.count (name) > 0;
        }

        int get_var (const std::string& name) const
        {
            auto it = vars_.find (name);
            if (it == vars_.end())
                throw RuntimeError ("undefined variable '" + name + "'");

            return it->second;
        }

        int get_var (const std::string& name, SourceLocation loc) const
        {
            auto it = vars_.find (name);
            if (it == vars_.end())
                throw RuntimeError (loc, "undefined variable '" + name + "'");

            return it->second;
        }

        void set_var (const std::string& name, int value)
        {
            vars_[name] = value;
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
