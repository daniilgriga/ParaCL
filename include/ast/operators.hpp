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
        Ne,   // !=
        And,  // &&
        Or,   // ||
        Xor   // ^
    };

    enum class UnOp
    {
        Neg,  // -
        Not   // !
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
            case BinOp::And: return "&&";
            case BinOp::Or:  return "||";
            case BinOp::Xor: return "^";
        }

        throw std::logic_error ("unknown BinOp");
    }

    inline std::string to_string (UnOp op)
    {
        switch (op)
        {
            case UnOp::Neg: return "-";
            case UnOp::Not: return "!";
        }

        throw std::logic_error ("unknown UnOp");
    }

} // namespace paracl
