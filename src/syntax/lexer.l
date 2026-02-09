%option c++ noyywrap yyclass="yy::Lexer"

%{
    #include "syntax/lexer.hpp"
%}

DIGIT  [0-9]
ID     [a-zA-Z_][a-zA-Z0-9_]*
WS     [ \t\r\n]+

%%

"print"         { advance_location(yytext); return process_print(); }
"if"            { advance_location(yytext); return process_if(); }
"else"          { advance_location(yytext); return process_else(); }
"while"         { advance_location(yytext); return process_while(); }

"=="            { advance_location(yytext); return process_eq(); }
"!="            { advance_location(yytext); return process_neq(); }
"<="            { advance_location(yytext); return process_less_or_eq(); }
">="            { advance_location(yytext); return process_greater_or_eq(); }
"<"             { advance_location(yytext); return process_less(); }
">"             { advance_location(yytext); return process_greater(); }

"+"             { advance_location(yytext); return process_add(); }
"-"             { advance_location(yytext); return process_sub(); }
"*"             { advance_location(yytext); return process_mul(); }
"/"             { advance_location(yytext); return process_div(); }
"%"             { advance_location(yytext); return process_mod(); }

"="             { advance_location(yytext); return process_assign(); }
";"             { advance_location(yytext); return process_scolon(); }
"("             { advance_location(yytext); return process_left_paren(); }
")"             { advance_location(yytext); return process_right_paren(); }
"{"             { advance_location(yytext); return process_left_brace(); }
"}"             { advance_location(yytext); return process_right_brace(); }
"?"             { advance_location(yytext); return process_input(); }

{DIGIT}+        { advance_location(yytext); return process_number(); }
{ID}            { advance_location(yytext); return process_var(); }

{WS}            { advance_location(yytext); }

.               { advance_location(yytext); return process_yyerror(); }

%%
