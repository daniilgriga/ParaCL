#pragma once

#include <string>
#include <stdexcept>

namespace paracl
{

    enum class BinOp
    {
        Add,  // +
        Sub,  // -
        Mul,  // *
        Div,  // /
        Mod,  // %
        Lt,   // <
        Le,   // <=
        Gt,   // >
        Ge,   // >=
        Eq,   // ==
        Ne    // !=
    };

    enum class UnOp
    {
        Neg   // -
    };

    inline std::string to_string (BinOp op)
    {
        switch (op)
        {
            case BinOp::Add: return "+";
            case BinOp::Sub: return "-";
            case BinOp::Mul: return "*";
            case BinOp::Div: return "/";
            case BinOp::Mod: return "%";
            case BinOp::Lt:  return "<";
            case BinOp::Le:  return "<=";
            case BinOp::Gt:  return ">";
            case BinOp::Ge:  return ">=";
            case BinOp::Eq:  return "==";
            case BinOp::Ne:  return "!=";
        }

        throw std::logic_error ("unknown BinOp");
    }

    inline std::string to_string (UnOp op)
    {
        switch (op)
        {
            case UnOp::Neg: return "-";
            // soon
        }

        throw std::logic_error ("unknown UnOp");
    }

} // namespace paracl
