#pragma once

#include <string>

#include <FlexLexer.h>

#include "parser.tab.hh"

namespace yy
{
    void set_location(parser::location_type* loc);

    class Driver
    {
        FlexLexer *plex_;
        std::string source_name_;

    public:
        explicit Driver (FlexLexer *plex, std::string source_name = "<stdin>")
            : plex_(plex), source_name_(source_name) {}

        parser::token_type yylex(parser::semantic_type *yylval,
                                 parser::location_type* yylloc)
        {
            if (yylloc && yylloc->begin.filename == nullptr)
            {
                yylloc->begin.filename = &source_name_;
                yylloc->end.filename = &source_name_;
            }

            set_location(yylloc);

            parser::token_type tt =
                static_cast<parser::token_type>(plex_->yylex());

            switch (tt)
            {
            case parser::token_type::NUMBER:
            {
                yylval->emplace<int>(std::stoi(plex_->YYText()));
                break;
            }
            case parser::token_type::VAR:
            {
                yylval->emplace<std::string>(plex_->YYText());
                break;
            }
            default:
                break;
            }

            return tt;
        }

        void error (const yy::parser::location_type& loc, const std::string& msg);

        bool parse()
        {
            parser parser(this);
            return parser.parse() == 0;
        }
    };

} // namespace yy
