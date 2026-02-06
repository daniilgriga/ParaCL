#pragma once

#include <string>

#include <FlexLexer.h>

#include "parser.tab.hh"

namespace yy
{

    class Driver
    {
    private:
        FlexLexer* plex_;

    public:
        explicit Driver (FlexLexer* plex) : plex_ (plex) {}

        parser::token_type yylex (parser::semantic_type* yylval)
        {
            parser::token_type tt =
                static_cast<parser::token_type> (plex_->yylex());

            switch (tt)
            {
                case parser::token_type::NUMBER:
                {
                    yylval->emplace<int> (std::stoi (plex_->YYText()));
                    break;
                }
                case parser::token_type::VAR:
                {
                    yylval->emplace<std::string> (plex_->YYText());
                    break;
                }
                default:
                    break;
            }

            return tt;
        }

        bool parse()
        {
            parser parser (this);
            return parser.parse() == 0;
        }
    };

} // namespace yy
