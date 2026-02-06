%option c++ noyywrap

%{
    #include "parser.tab.hh"
%}

DIGIT  [0-9]
ID     [a-zA-Z_][a-zA-Z0-9_]*
WS     [ \t\r\n]+

%%

"print"         { return yy::parser::token_type::PRINT; }
"if"            { return yy::parser::token_type::IF; }
"else"          { return yy::parser::token_type::ELSE; }
"while"         { return yy::parser::token_type::WHILE; }

"=="            { return yy::parser::token_type::EQ; }
"!="            { return yy::parser::token_type::NEQ; }
"<="            { return yy::parser::token_type::LESS_OR_EQ; }
">="            { return yy::parser::token_type::GREATER_OR_EQ; }
"<"             { return yy::parser::token_type::LESS; }
">"             { return yy::parser::token_type::GREATER; }

"+"             { return yy::parser::token_type::ADD; }
"-"             { return yy::parser::token_type::SUB; }
"*"             { return yy::parser::token_type::MUL; }
"/"             { return yy::parser::token_type::DIV; }
"%"             { return yy::parser::token_type::MOD; }

"="             { return yy::parser::token_type::ASSIGN; }
";"             { return yy::parser::token_type::SCOLON; }
"("             { return yy::parser::token_type::LEFT_PAREN; }
")"             { return yy::parser::token_type::RIGHT_PAREN; }
"{"             { return yy::parser::token_type::LEFT_BRACE; }
"}"             { return yy::parser::token_type::RIGHT_BRACE; }
"?"             { return yy::parser::token_type::INPUT; }

{DIGIT}+        { return yy::parser::token_type::NUMBER; }
{ID}            { return yy::parser::token_type::VAR; }

{WS}            { /* skip whitespace */ }

.               { return yy::parser::token_type::YYerror; }

%%
