#pragma once

#include <charconv>
#include <cstring>
#include <string>

#include "ast/ast_builder.hpp"
#include "errors/errors.hpp"
#include "parser.tab.hh"
#include "syntax/lexer.hpp"

namespace yy
{
    class Driver
    {
        Lexer* plex_;
        std::string source_name_;
        paracl::AstBuilder builder_;

        void throw_syntax_error (parser::location_type* yylloc,
                                 const std::string& msg)
        {
            if (yylloc)
                error (*yylloc, msg);

            parser::location_type fallback;
            fallback.begin.filename = &source_name_;
            fallback.end.filename = &source_name_;
            error (fallback, msg);
        }

    public:
        explicit Driver (Lexer* plex, std::string source_name = "<stdin>")
            : plex_(plex), source_name_(source_name) {}

        parser::token_type yylex (parser::semantic_type* yylval,
                                  parser::location_type* yylloc)
        {
            if (yylloc && yylloc->begin.filename == nullptr)
            {
                yylloc->begin.filename = &source_name_;
                yylloc->end.filename = &source_name_;
            }

            plex_->set_context (yylloc);

            parser::token_type tt =
                static_cast<parser::token_type> (plex_->yylex());

            if (tt == parser::token_type::YYerror)
            {
                const std::string bad = plex_->text() ? plex_->text() : "";
                const std::string msg = bad.empty()
                    ? "invalid token"
                    : "invalid token '" + bad + "'";
                throw_syntax_error (yylloc, msg);
            }

            switch (tt)
            {
                case parser::token_type::NUMBER:
                {
                    const char* text = plex_->text() ? plex_->text() : "";
                    const size_t len = std::strlen (text);
                    const std::string bad (text, len);

                    int value = 0;
                    const auto [ptr, ec] = std::from_chars (text, text + len, value);
                    if (ec == std::errc::result_out_of_range)
                        throw_syntax_error (yylloc, "integer literal out of range '" + bad + "'");

                    if (ec != std::errc{} || ptr != text + len)
                        throw_syntax_error (yylloc, "invalid integer literal '" + bad + "'");

                    yylval->emplace<int> (value);
                    break;
                }
                case parser::token_type::VAR:
                {
                    yylval->emplace<std::string> (plex_->text());
                    break;
                }
                default:
                    break;
            }

            return tt;
        }

        void error (const yy::parser::location_type& loc, const std::string& msg)
        {
            const paracl::SourceLocation where
            {
                loc.begin.line,
                loc.begin.column,
                source_name_
            };

            throw paracl::SyntaxError (where, msg);
        }

        paracl::AstBuilder& builder() & { return builder_; }
        const paracl::AstBuilder& builder() const & { return builder_; }

        void set_root (const paracl::Stmt* root) { builder_.set_root (root); }
        const paracl::Stmt* root() const { return builder_.root(); }

        void parse()
        {
            parser parser_ (this);
            const int rc = parser_.parse();

            if (rc != 0)
            {
                throw paracl::SyntaxError (
                    "failed to parse '" + source_name_ +
                    "': syntax analysis failed (rc=" + std::to_string (rc) + ")"
                );
            }

            if (root() == nullptr)
            {
                throw paracl::SyntaxError (
                    "failed to parse '" + source_name_ + "' - no program to execute"
                );
            }
        }
    };

} // namespace yy
