#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace paracl
{

    struct SourceLocation
    {
        int line = 1;
        int column = 1;
        std::string file = "";
    };

    inline std::string format_location (SourceLocation loc)
    {
        if (loc.line <= 0)
            return "";

        std::string file_prefix = loc.file.empty() ? "" : (loc.file + ":");

        if (loc.column <= 0)
            return " (" + file_prefix + std::to_string(loc.line) + ")";

        return " (" + file_prefix + std::to_string(loc.line) + ":" + std::to_string(loc.column) + ")";
    }


    class ParaCLError : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error; // inheriting constructors
    };

    enum class ErrorType { Syntax, Runtime };

    // format: <prefix> <location>: <message>
    inline std::string make_error_message (ErrorType type, SourceLocation loc, std::string_view msg)
    {
        const char* prefix = (type == ErrorType::Syntax) ? "Syntax error" : "Runtime error";

        std::string out = prefix;
        out += format_location (loc);
        out += ": ";
        out += msg;

        return out;
    }

    class SyntaxError final : public ParaCLError
    {
    public:
        explicit SyntaxError (std::string_view msg)
            : ParaCLError (make_error_message (ErrorType::Syntax, {0, 0}, msg)) {}

        SyntaxError (SourceLocation loc, std::string_view msg)
            : ParaCLError (make_error_message (ErrorType::Syntax, loc, msg)) {}
    };

    class RuntimeError final : public ParaCLError
    {
    public:
        explicit RuntimeError (std::string_view msg)
            : ParaCLError (make_error_message (ErrorType::Runtime, {0, 0}, msg)) {}

        RuntimeError (SourceLocation loc, std::string_view msg)
            : ParaCLError (make_error_message (ErrorType::Runtime, loc, msg)) {}
    };

} // namespace paracl
