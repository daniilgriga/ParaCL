#include <gtest/gtest.h>

#include <sstream>

#include "ast/ast_builder.hpp"
#include "ast/expr_nodes.hpp"
#include "ast/stmt_nodes.hpp"
#include "interpreter/context.hpp"

TEST (AstBuilderTest, RootIsNullByDefault)
{
    paracl::AstBuilder builder;
    EXPECT_EQ (builder.root(), nullptr);
}

TEST (AstBuilderTest, MakeExprReturnsValidPointer)
{
    paracl::AstBuilder builder;
    const auto* lit = builder.make_expr<paracl::IntLiteral> (13);

    ASSERT_NE (lit, nullptr);

    paracl::Context ctx;
    EXPECT_EQ (lit->eval (ctx), 13);
}

TEST (AstBuilderTest, MakeStmtReturnsValidPointer)
{
    paracl::AstBuilder builder;
    const auto* lit = builder.make_expr<paracl::IntLiteral> (7);
    const auto* print = builder.make_stmt<paracl::PrintStmt> (lit);

    ASSERT_NE (print, nullptr);
}

TEST (AstBuilderTest, SetRootAndRetrieve)
{
    paracl::AstBuilder builder;
    const auto* lit = builder.make_expr<paracl::IntLiteral> (1);
    const auto* stmt = builder.make_stmt<paracl::PrintStmt> (lit);

    builder.set_root (stmt);
    EXPECT_EQ (builder.root(), stmt);
}

TEST (AstBuilderTest, FullPipeline)
{
    paracl::AstBuilder builder;

    // x = 5;
    const auto* five = builder.make_expr<paracl::IntLiteral> (5);
    const auto* assign = builder.make_stmt<paracl::AssignStmt> ("x", five);

    // print (x + 1);
    const auto* var_x = builder.make_expr<paracl::VarRef> ("x");
    const auto* one = builder.make_expr<paracl::IntLiteral> (1);
    const auto* add = builder.make_expr<paracl::BinaryExpr> (paracl::BinOp::Add, var_x, one);
    const auto* print = builder.make_stmt<paracl::PrintStmt> (add);

    // { x = 5; print (x + 1); }
    std::vector<const paracl::Stmt*> stmts = {assign, print};
    const auto* block = builder.make_stmt<paracl::BlockStmt> (std::move (stmts));

    builder.set_root (block);

    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    builder.root()->exec (ctx);
    EXPECT_EQ (out.str(), "6\n");
}
