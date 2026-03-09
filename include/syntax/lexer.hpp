#pragma once

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif

#include <FlexLexer.h>
#include <istream>

#include "parser.tab.hh"

namespace yy
{
    class Lexer : public yyFlexLexer
    {
    private:
        parser::location_type* yylloc_ = nullptr;

        void advance_location (const char* text, int len)
        {
            if (!yylloc_ || !text || len <= 0)
                return;

            yylloc_->begin = yylloc_->end;

            for (int i = 0; i < len; ++i)
            {
                if (text[i] == '\n')
                    yylloc_->end.lines(1);
                else
                    yylloc_->end.columns(1);
            }
        }

    public:
        explicit Lexer(std::istream* in) : yyFlexLexer(in) {}

        void set_context (parser::location_type* yylloc)
        {
            yylloc_ = yylloc;
        }

        const char* text() const { return YYText(); }

        int yylex() override;
    };
} // namespace yy
