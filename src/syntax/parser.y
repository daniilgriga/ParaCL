%language "c++"
%skeleton "lalr1.cc"
%defines
%locations
%define api.value.type variant
%param {yy::Driver* driver}
%define parse.error verbose

%code requires
{
  #include <string_view>
  #include <string>
  #include <vector>

  #include "ast/expr.hpp"
  #include "ast/stmt.hpp"

  namespace yy { class Driver; }
}

%code
{
  #include <utility>

  #include "ast/expr_nodes.hpp"
  #include "ast/stmt_nodes.hpp"
  #include "syntax/driver.hpp"

  namespace yy
  {
    static paracl::SourceLocation make_loc(const parser::location_type& loc)
    {
      return paracl::SourceLocation{
        loc.begin.line,
        loc.begin.column,
        (loc.begin.filename != nullptr)
            ? std::string_view(*loc.begin.filename)
            : std::string_view{}
      };
    }

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

%token AND             "&&"
%token OR              "||"
%token NOT             "!"
%token XOR             "^" 

%token  <int>          NUMBER
%token  <std::string>  VAR
%nterm  <const paracl::Stmt*>                    program
%nterm  <std::vector<const paracl::Stmt*>>       stmt_list
%nterm  <const paracl::Stmt*>                    stmt
%nterm  <const paracl::Expr*>                    expr
%nterm  <const paracl::Expr*>                    or_expr
%nterm  <const paracl::Expr*>                    xor_expr
%nterm  <const paracl::Expr*>                    and_expr
%nterm  <const paracl::Expr*>                    cmp_expr
%nterm  <const paracl::Expr*>                    add_expr
%nterm  <const paracl::Expr*>                    mul_expr
%nterm  <const paracl::Expr*>                    unary_expr
%nterm  <const paracl::Expr*>                    primary

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
    {
      $$ = driver->builder().make_stmt<paracl::BlockStmt>(
          std::move($1), yy::make_loc(@$));
      driver->set_root($$);
    }
  ;

stmt_list
  : stmt_list stmt
    {
      auto out = std::move($1);
      out.push_back($2);
      $$ = std::move(out);
    }
  | %empty
    {
      $$ = {};
    }
  ;

stmt
  : PRINT expr SCOLON
    {
      $$ = driver->builder().make_stmt<paracl::PrintStmt>(
          $2, yy::make_loc(@$));
    }
  | expr SCOLON
    {
      $$ = driver->builder().make_stmt<paracl::ExprStmt>(
          $1, yy::make_loc(@$));
    }
  | LEFT_BRACE stmt_list RIGHT_BRACE
    {
      $$ = driver->builder().make_stmt<paracl::BlockStmt>(
          std::move($2), yy::make_loc(@$));
    }
  | IF LEFT_PAREN expr RIGHT_PAREN stmt %prec LOWER_THAN_ELSE
    {
      $$ = driver->builder().make_stmt<paracl::IfStmt>(
          $3, $5, nullptr, yy::make_loc(@$));
    }
  | IF LEFT_PAREN expr RIGHT_PAREN stmt ELSE stmt
    {
      $$ = driver->builder().make_stmt<paracl::IfStmt>(
          $3, $5, $7, yy::make_loc(@$));
    }
  | WHILE LEFT_PAREN expr RIGHT_PAREN stmt
    {
      $$ = driver->builder().make_stmt<paracl::WhileStmt>(
          $3, $5, yy::make_loc(@$));
    }
  ;

expr
  : VAR ASSIGN expr
    {
      $$ = driver->builder().make_expr<paracl::AssignExpr>(
          std::move($1), $3, yy::make_loc(@$));
    }
  | or_expr
    {
      $$ = $1;
    }
  ;

or_expr
  : or_expr OR xor_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Or, $1, $3, yy::make_loc(@$));
    }
  | xor_expr
    {
      $$ = $1;
    }
  ;

xor_expr
  : xor_expr XOR and_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Xor, $1, $3, yy::make_loc(@$));
    }
  | and_expr
    {
      $$ = $1;
    }
  ;

and_expr
  : and_expr AND cmp_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::And, $1, $3, yy::make_loc(@$));
    }
  | cmp_expr
    {
      $$ = $1;
    }
  ;

cmp_expr
  : add_expr
    {
      $$ = $1;
    }
  | add_expr EQ add_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Eq, $1, $3, yy::make_loc(@$));
    }
  | add_expr NEQ add_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Ne, $1, $3, yy::make_loc(@$));
    }
  | add_expr LESS add_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Lt, $1, $3, yy::make_loc(@$));
    }
  | add_expr GREATER add_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Gt, $1, $3, yy::make_loc(@$));
    }
  | add_expr LESS_OR_EQ add_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Le, $1, $3, yy::make_loc(@$));
    }
  | add_expr GREATER_OR_EQ add_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Ge, $1, $3, yy::make_loc(@$));
    }
  ;

add_expr
  : add_expr ADD mul_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Add, $1, $3, yy::make_loc(@$));
    }
  | add_expr SUB mul_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Sub, $1, $3, yy::make_loc(@$));
    }
  | mul_expr
    {
      $$ = $1;
    }
  ;

mul_expr
  : mul_expr MUL unary_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Mul, $1, $3, yy::make_loc(@$));
    }
  | mul_expr DIV unary_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Div, $1, $3, yy::make_loc(@$));
    }
  | mul_expr MOD unary_expr
    {
      $$ = driver->builder().make_expr<paracl::BinaryExpr>(
          paracl::BinOp::Mod, $1, $3, yy::make_loc(@$));
    }
  | unary_expr
    {
      $$ = $1;
    }
  ;

unary_expr
  : NOT unary_expr
    {
      $$ = driver->builder().make_expr<paracl::UnaryExpr>(
          paracl::UnOp::Not, $2, yy::make_loc(@$));
    }
  | SUB unary_expr
    {
      $$ = driver->builder().make_expr<paracl::UnaryExpr>(
          paracl::UnOp::Neg, $2, yy::make_loc(@$));
    }
  | primary
    {
      $$ = $1;
    }
  ;

primary
  : NUMBER
    {
      $$ = driver->builder().make_expr<paracl::IntLiteral>(
          $1, yy::make_loc(@$));
    }
  | VAR
    {
      $$ = driver->builder().make_expr<paracl::VarRef>(
          std::move($1), yy::make_loc(@$));
    }
  | INPUT
    {
      $$ = driver->builder().make_expr<paracl::ReadExpr>(
          yy::make_loc(@$));
    }
  | LEFT_PAREN expr RIGHT_PAREN
    {
      $$ = $2;
    }
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
