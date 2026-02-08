%option c++ noyywrap

%{
    #include "parser.tab.hh"

    namespace yy
    {
        parser::location_type* current_location = nullptr;

        void set_location (parser::location_type* loc)
        {
            current_location = loc;
        }

        void advance_location (const char* text)
        {
            if (current_location == nullptr || text == nullptr)
            {
                return;
            }

            current_location->begin = current_location->end;
            for (int i = 0; text[i] != '\0'; ++i)
            {
                if (text[i] == '\n')
                {
                    current_location->end.lines(1);
                }
                else
                {
                    current_location->end.columns(1);
                }
            }
        }
    }  // namespace yy
%}

DIGIT  [0-9]
ID     [a-zA-Z_][a-zA-Z0-9_]*
WS     [ \t\r\n]+

%%

"print"         { yy::advance_location(yytext); return yy::parser::token_type::PRINT; }
"if"            { yy::advance_location(yytext); return yy::parser::token_type::IF; }
"else"          { yy::advance_location(yytext); return yy::parser::token_type::ELSE; }
"while"         { yy::advance_location(yytext); return yy::parser::token_type::WHILE; }

"=="            { yy::advance_location(yytext); return yy::parser::token_type::EQ; }
"!="            { yy::advance_location(yytext); return yy::parser::token_type::NEQ; }
"<="            { yy::advance_location(yytext); return yy::parser::token_type::LESS_OR_EQ; }
">="            { yy::advance_location(yytext); return yy::parser::token_type::GREATER_OR_EQ; }
"<"             { yy::advance_location(yytext); return yy::parser::token_type::LESS; }
">"             { yy::advance_location(yytext); return yy::parser::token_type::GREATER; }

"+"             { yy::advance_location(yytext); return yy::parser::token_type::ADD; }
"-"             { yy::advance_location(yytext); return yy::parser::token_type::SUB; }
"*"             { yy::advance_location(yytext); return yy::parser::token_type::MUL; }
"/"             { yy::advance_location(yytext); return yy::parser::token_type::DIV; }
"%"             { yy::advance_location(yytext); return yy::parser::token_type::MOD; }

"="             { yy::advance_location(yytext); return yy::parser::token_type::ASSIGN; }
";"             { yy::advance_location(yytext); return yy::parser::token_type::SCOLON; }
"("             { yy::advance_location(yytext); return yy::parser::token_type::LEFT_PAREN; }
")"             { yy::advance_location(yytext); return yy::parser::token_type::RIGHT_PAREN; }
"{"             { yy::advance_location(yytext); return yy::parser::token_type::LEFT_BRACE; }
"}"             { yy::advance_location(yytext); return yy::parser::token_type::RIGHT_BRACE; }
"?"             { yy::advance_location(yytext); return yy::parser::token_type::INPUT; }

{DIGIT}+        { yy::advance_location(yytext); return yy::parser::token_type::NUMBER; }
{ID}            { yy::advance_location(yytext); return yy::parser::token_type::VAR; }

{WS}            { yy::advance_location(yytext); }

.               { yy::advance_location(yytext); return yy::parser::token_type::YYerror; }

%%
