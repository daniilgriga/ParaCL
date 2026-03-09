%option c++ noyywrap yyclass="yy::Lexer"

%{
    #include "syntax/lexer.hpp"
%}

DIGIT  [0-9]
DIGIT1 [1-9]
ID     [a-zA-Z_][a-zA-Z0-9_]*
WS     [ \t\r\n]+
LINE_COMMENT "//"[^\n]*

%%

"print"         { advance_location(yytext, yyleng); return yy::parser::token_type::PRINT; }
"if"            { advance_location(yytext, yyleng); return yy::parser::token_type::IF; }
"else"          { advance_location(yytext, yyleng); return yy::parser::token_type::ELSE; }
"while"         { advance_location(yytext, yyleng); return yy::parser::token_type::WHILE; }

"=="            { advance_location(yytext, yyleng); return yy::parser::token_type::EQ; }
"!="            { advance_location(yytext, yyleng); return yy::parser::token_type::NEQ; }
"<="            { advance_location(yytext, yyleng); return yy::parser::token_type::LESS_OR_EQ; }
">="            { advance_location(yytext, yyleng); return yy::parser::token_type::GREATER_OR_EQ; }
"<"             { advance_location(yytext, yyleng); return yy::parser::token_type::LESS; }
">"             { advance_location(yytext, yyleng); return yy::parser::token_type::GREATER; }

"&&"            { advance_location(yytext, yyleng); return yy::parser::token_type::AND; }
"||"            { advance_location(yytext, yyleng); return yy::parser::token_type::OR; }
"!"             { advance_location(yytext, yyleng); return yy::parser::token_type::NOT; }
"^"             { advance_location(yytext, yyleng); return yy::parser::token_type::XOR; }

"+"             { advance_location(yytext, yyleng); return yy::parser::token_type::ADD; }
"-"             { advance_location(yytext, yyleng); return yy::parser::token_type::SUB; }
"*"             { advance_location(yytext, yyleng); return yy::parser::token_type::MUL; }
"/"             { advance_location(yytext, yyleng); return yy::parser::token_type::DIV; }
"%"             { advance_location(yytext, yyleng); return yy::parser::token_type::MOD; }

"="             { advance_location(yytext, yyleng); return yy::parser::token_type::ASSIGN; }
";"             { advance_location(yytext, yyleng); return yy::parser::token_type::SCOLON; }
"("             { advance_location(yytext, yyleng); return yy::parser::token_type::LEFT_PAREN; }
")"             { advance_location(yytext, yyleng); return yy::parser::token_type::RIGHT_PAREN; }
"{"             { advance_location(yytext, yyleng); return yy::parser::token_type::LEFT_BRACE; }
"}"             { advance_location(yytext, yyleng); return yy::parser::token_type::RIGHT_BRACE; }
"?"             { advance_location(yytext, yyleng); return yy::parser::token_type::INPUT; }

"0"             { advance_location(yytext, yyleng); return yy::parser::token_type::NUMBER; }
{DIGIT1}{DIGIT}*    { advance_location(yytext, yyleng); return yy::parser::token_type::NUMBER; }
{ID}            { advance_location(yytext, yyleng); return yy::parser::token_type::VAR; }

{WS}            { advance_location(yytext, yyleng); }
{LINE_COMMENT}  { advance_location(yytext, yyleng); }

"0"{DIGIT}+     { advance_location(yytext, yyleng); return yy::parser::token_type::YYerror; }
.               { advance_location(yytext, yyleng); return yy::parser::token_type::YYerror; }

%%
