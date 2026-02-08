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
%nterm  <int>          equals
%nterm  <int>          expr

%right ASSIGN
%left ADD SUB
%left MUL DIV
%nonassoc EQ NEQ LESS GREATER LESS_OR_EQ GREATER_OR_EQ

%start program

%%

program 
  : stmt_list
  ;

stmt_list
  : stmt_list stmt
  | stmt
  ;

stmt
  : VAR ASSIGN expr SCOLON
  | PRINT expr SCOLON
  | expr SCOLON
  ;

expr
  : NUMBER                      { $$ = $1; }
  | VAR                         { $$ = 0; }       // plug
  | expr ADD expr               { $$ = $1 + $3; }
  | expr SUB expr               { $$ = $1 - $3; }
  | expr MUL expr               { $$ = $1 * $3; }
  | expr DIV expr               { $$ = $1 / $3; }
  | expr MOD expr               { $$ = $1 % $3; }
  | LEFT_PAREN expr RIGHT_PAREN { $$ = $2; }
  | equals                      { $$ = $1; }
  ;

equals
  : expr EQ expr            { $$ = $1 == $3; }
  | expr NEQ expr           { $$ = $1 != $3; }
  | expr LESS expr          { $$ = $1 < $3;  }
  | expr GREATER expr       { $$ = $1 > $3;  }
  | expr LESS_OR_EQ expr    { $$ = $1 <= $3; }
  | expr GREATER_OR_EQ expr { $$ = $1 >= $3; }
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
