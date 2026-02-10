%language "c++"
%skeleton "lalr1.cc"
%defines
%locations
%define api.value.type variant
%param {yy::Driver* driver}
%define parse.error verbose

%code requires
{
  #include <string>

  namespace yy { class Driver; }
}

%code
{
  #include <iostream>
  #include "driver.hpp"

  namespace yy
  {
    parser::token_type yylex (parser::semantic_type* yylval,
                              parser::location_type* yylloc,
                              Driver* driver);
  }
}

%token  ADD            "+"
%token  SUB            "-"
%token  MUL            "*"
%token  DIV            "/"
%token  MOD            "%"

%token  ASSIGN         "="
%token  SCOLON         ";"
%token  LEFT_PAREN     "("
%token  RIGHT_PAREN    ")"
%token  LEFT_BRACE     "{"
%token  RIGHT_BRACE    "}"

%token  INPUT          "?"
%token  PRINT          "print"
%token  IF             "if"
%token  ELSE           "else"
%token  WHILE          "while"

%token  EQ             "=="
%token  NEQ            "!="
%token  LESS           "<"
%token  GREATER        ">"
%token  LESS_OR_EQ     "<="
%token  GREATER_OR_EQ  ">="

%token  <int>          NUMBER
%token  <std::string>  VAR
%nterm  <int>          expr
%nterm  <int>          cmp_expr
%nterm  <int>          add_expr
%nterm  <int>          mul_expr
%nterm  <int>          unary_expr
%nterm  <int>          primary

%right ASSIGN
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left ADD SUB
%left MUL DIV MOD
%nonassoc EQ NEQ LESS GREATER LESS_OR_EQ GREATER_OR_EQ

%start program

%%

program 
  : stmt_list
  ;

stmt_list
  : stmt_list stmt
  | %empty
  ;

stmt
  : VAR ASSIGN expr SCOLON
  | PRINT expr SCOLON
  | expr SCOLON
  | LEFT_BRACE stmt_list RIGHT_BRACE
  | IF LEFT_PAREN expr RIGHT_PAREN stmt %prec LOWER_THAN_ELSE
  | IF LEFT_PAREN expr RIGHT_PAREN stmt ELSE stmt
  | WHILE LEFT_PAREN expr RIGHT_PAREN stmt
  ;

expr
  : cmp_expr                    { $$ = $1; }
  ;

cmp_expr
  : add_expr                        { $$ = $1; }
  | add_expr EQ add_expr            { $$ = $1 == $3; }
  | add_expr NEQ add_expr           { $$ = $1 != $3; }
  | add_expr LESS add_expr          { $$ = $1 < $3; }
  | add_expr GREATER add_expr       { $$ = $1 > $3; }
  | add_expr LESS_OR_EQ add_expr    { $$ = $1 <= $3; }
  | add_expr GREATER_OR_EQ add_expr { $$ = $1 >= $3; }
  ;

add_expr
  : add_expr ADD mul_expr           { $$ = $1 + $3; }
  | add_expr SUB mul_expr           { $$ = $1 - $3; }
  | mul_expr                        { $$ = $1; }
  ;

mul_expr
  : mul_expr MUL unary_expr         { $$ = $1 * $3; }
  | mul_expr DIV unary_expr         { $$ = $1 / $3; }
  | mul_expr MOD unary_expr         { $$ = $1 % $3; }
  | unary_expr                      { $$ = $1; }
  ;

unary_expr
  : SUB unary_expr                  { $$ = -$2; }
  | primary                         { $$ = $1; }
  ;

primary
  : NUMBER                          { $$ = $1; }
  | VAR                             { $$ = 0; }       // plug
  | LEFT_PAREN expr RIGHT_PAREN     { $$ = $2; }
  ;

%%

namespace yy
{
    parser::token_type yylex (parser::semantic_type* yylval,
                             parser::location_type* yylloc,
                             Driver* driver)
    {
        return driver->yylex(yylval, yylloc);
    }

    void parser::error (const location_type& yyloc, const std::string& msg)
    {
        driver->error(yyloc, msg);
    }

}  // namespace yy
