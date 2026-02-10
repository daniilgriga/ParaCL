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

        void advance_location (const char* text)
        {
            if (!yylloc_ || !text)
                return;

            yylloc_->begin = yylloc_->end;

            for (int i = 0; text[i] != '\0'; ++i)
            {
                if (text[i] == '\n')
                {
                    yylloc_->end.lines(1);
                }
                else
                {
                    yylloc_->end.columns(1);
                }
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

        parser::token_type process_print() const { return yy::parser::token_type::PRINT; }
        parser::token_type process_if() const { return yy::parser::token_type::IF; }
        parser::token_type process_else() const { return yy::parser::token_type::ELSE; }
        parser::token_type process_while() const { return yy::parser::token_type::WHILE; }

        parser::token_type process_eq() const { return yy::parser::token_type::EQ; }
        parser::token_type process_neq() const { return yy::parser::token_type::NEQ; }
        parser::token_type process_less_or_eq() const { return yy::parser::token_type::LESS_OR_EQ; }
        parser::token_type process_greater_or_eq() const { return yy::parser::token_type::GREATER_OR_EQ; }
        parser::token_type process_less() const { return yy::parser::token_type::LESS; }
        parser::token_type process_greater() const { return yy::parser::token_type::GREATER; }
        
        parser::token_type process_add() const { return yy::parser::token_type::ADD; }
        parser::token_type process_sub() const { return yy::parser::token_type::SUB; }
        parser::token_type process_mul() const { return yy::parser::token_type::MUL; }
        parser::token_type process_div() const { return yy::parser::token_type::DIV; }
        parser::token_type process_mod() const { return yy::parser::token_type::MOD; }

        parser::token_type process_assign() const { return yy::parser::token_type::ASSIGN; }
        parser::token_type process_scolon() const { return yy::parser::token_type::SCOLON; }
        parser::token_type process_left_paren() const { return yy::parser::token_type::LEFT_PAREN; }
        parser::token_type process_right_paren() const { return yy::parser::token_type::RIGHT_PAREN; }
        parser::token_type process_left_brace() const { return yy::parser::token_type::LEFT_BRACE; }
        parser::token_type process_right_brace() const { return yy::parser::token_type::RIGHT_BRACE; }
        parser::token_type process_input() const { return yy::parser::token_type::INPUT; }

        parser::token_type process_number() const { return yy::parser::token_type::NUMBER; }
        parser::token_type process_var() const { return yy::parser::token_type::VAR; }

        parser::token_type process_yyerror() const { return yy::parser::token_type::YYerror; }
    };
} // namespace yy
