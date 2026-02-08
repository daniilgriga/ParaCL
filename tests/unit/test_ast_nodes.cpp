#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ast/expr_nodes.hpp"
#include "ast/stmt_nodes.hpp"

class TestArena // temporary ownership for tests
{
private:
    std::vector<std::unique_ptr<paracl::Expr>> exprs_;
    std::vector<std::unique_ptr<paracl::Stmt>> stmts_;

public:
    template <typename T, typename... Args>
    const T* make_expr (Args&&... args)
    {
        auto ptr = std::make_unique<T> (std::forward<Args> (args)...);
        const T* raw = ptr.get();
        exprs_.push_back (std::move (ptr));

        return raw;
    }

    template <typename T, typename... Args>
    const T* make_stmt (Args&&... args)
    {
        auto ptr = std::make_unique<T> (std::forward<Args> (args)...);
        const T* raw = ptr.get();
        stmts_.push_back (std::move (ptr));

        return raw;
    }
};

TEST (IntLiteralTest, ReturnsValue)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    paracl::IntLiteral lit (52);
    EXPECT_EQ (lit.eval (ctx), 52);
}

TEST (IntLiteralTest, NegativeValue)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    paracl::IntLiteral lit (-7);
    EXPECT_EQ (lit.eval (ctx), -7);
}

TEST (VarRefTest, ReturnsVarValue)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    ctx.set_var ("x", 10);
    paracl::VarRef ref ("x");
    EXPECT_EQ (ref.eval (ctx), 10);
}

TEST (VarRefTest, UndefinedVarThrows)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    paracl::VarRef ref ("x");
    EXPECT_THROW (ref.eval (ctx), paracl::RuntimeError);
}

TEST (ReadExprTest, ReadsFromInput)
{
    std::istringstream in ("99");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    paracl::ReadExpr read;
    EXPECT_EQ (read.eval (ctx), 99);
}

TEST (ReadExprTest, InvalidInputThrows)
{
    std::istringstream in ("bac");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    paracl::ReadExpr read;
    EXPECT_THROW (read.eval (ctx), paracl::RuntimeError);
}

TEST (UnaryExprTest, Negation)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* lit = arena.make_expr<paracl::IntLiteral> (5);
    paracl::UnaryExpr neg (paracl::UnOp::Neg, lit);

    EXPECT_EQ (neg.eval (ctx), -5);
}

TEST (BinaryExprTest, Addition)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* lhs = arena.make_expr<paracl::IntLiteral> (3);
    auto* rhs = arena.make_expr<paracl::IntLiteral> (7);
    paracl::BinaryExpr add (paracl::BinOp::Add, lhs, rhs);

    EXPECT_EQ (add.eval (ctx), 10);
}

TEST (BinaryExprTest, DivisionByZeroThrows)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* lhs = arena.make_expr<paracl::IntLiteral> (10);
    auto* rhs = arena.make_expr<paracl::IntLiteral> (0);
    paracl::BinaryExpr div (paracl::BinOp::Div, lhs, rhs);

    EXPECT_THROW (div.eval (ctx), paracl::RuntimeError);
}

TEST (BinaryExprTest, ModByZeroThrows)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* lhs = arena.make_expr<paracl::IntLiteral> (10);
    auto* rhs = arena.make_expr<paracl::IntLiteral> (0);
    paracl::BinaryExpr mod (paracl::BinOp::Mod, lhs, rhs);

    EXPECT_THROW (mod.eval (ctx), paracl::RuntimeError);
}

TEST (BinaryExprTest, Comparison)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* lhs = arena.make_expr<paracl::IntLiteral> (5);
    auto* rhs = arena.make_expr<paracl::IntLiteral> (3);

    paracl::BinaryExpr lt (paracl::BinOp::Lt, lhs, rhs);
    EXPECT_EQ (lt.eval (ctx), 0);

    paracl::BinaryExpr gt (paracl::BinOp::Gt, lhs, rhs);
    EXPECT_EQ (gt.eval (ctx), 1);

    paracl::BinaryExpr eq (paracl::BinOp::Eq, lhs, rhs);
    EXPECT_EQ (eq.eval (ctx), 0);
}

TEST (AssignStmtTest, AssignsVariable)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* val = arena.make_expr<paracl::IntLiteral> (42);

    paracl::AssignStmt assign ("x", val);
    assign.exec (ctx);

    EXPECT_EQ (ctx.get_var ("x"), 42);
}

TEST (PrintStmtTest, PrintsValue)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* val = arena.make_expr<paracl::IntLiteral> (9);

    paracl::PrintStmt print (val);
    print.exec (ctx);

    EXPECT_EQ (out.str(), "9\n");
}

TEST (ExprStmtTest, EvaluatesExpression)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* val = arena.make_expr<paracl::IntLiteral> (42);

    paracl::ExprStmt expr_stmt (val);
    expr_stmt.exec (ctx);

    EXPECT_EQ (out.str(), "");
}

TEST (ExprStmtTest, EvaluatesWithSideEffect)
{
    std::istringstream in ("7");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* read = arena.make_expr<paracl::ReadExpr>();

    paracl::ExprStmt expr_stmt (read);
    expr_stmt.exec (ctx);

    // read consumed the input — next read should throw
    EXPECT_THROW (ctx.read_int(), paracl::RuntimeError);
}

TEST (BlockStmtTest, ExecutesInOrder)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* val1 = arena.make_expr<paracl::IntLiteral> (1);
    auto* val2 = arena.make_expr<paracl::IntLiteral> (2);
    auto* print1 = arena.make_stmt<paracl::PrintStmt> (val1);
    auto* print2 = arena.make_stmt<paracl::PrintStmt> (val2);

    paracl::BlockStmt block ({print1, print2});
    block.exec (ctx);

    EXPECT_EQ (out.str(), "1\n2\n");
}

// x = 2 + 3;
// print x;
TEST (IntegrationTest, AssignAndPrint)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* two = arena.make_expr<paracl::IntLiteral> (2);
    auto* three = arena.make_expr<paracl::IntLiteral> (3);
    auto* sum = arena.make_expr<paracl::BinaryExpr> (paracl::BinOp::Add, two, three);
    auto* var_x = arena.make_expr<paracl::VarRef> ("x");

    auto* assign = arena.make_stmt<paracl::AssignStmt> ("x", sum);
    auto* print = arena.make_stmt<paracl::PrintStmt> (var_x);

    paracl::BlockStmt program ({assign, print});
    program.exec (ctx);

    EXPECT_EQ (out.str(), "5\n");
}

// x = ?;   (input: 49)
// print x;
TEST (IntegrationTest, ReadAndPrint)
{
    std::istringstream in ("49");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* read = arena.make_expr<paracl::ReadExpr>();
    auto* var_x = arena.make_expr<paracl::VarRef> ("x");

    auto* assign = arena.make_stmt<paracl::AssignStmt> ("x", read);
    auto* print = arena.make_stmt<paracl::PrintStmt> (var_x);

    paracl::BlockStmt program ({assign, print});
    program.exec (ctx);

    EXPECT_EQ (out.str(), "49\n");
}

// print 10 / 0;
TEST (IntegrationTest, DivisionByZeroWithLocation)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* ten = arena.make_expr<paracl::IntLiteral> (10);
    auto* zero = arena.make_expr<paracl::IntLiteral> (0);
    auto* div = arena.make_expr<paracl::BinaryExpr> (
        paracl::BinOp::Div, ten, zero, paracl::SourceLocation{1, 5});

    paracl::PrintStmt print (div);

    try
    {
        print.exec (ctx);
        FAIL() << "Expected RuntimeError";
    }
    catch (const paracl::RuntimeError& e)
    {
        EXPECT_EQ (std::string (e.what()), "Runtime error (1:5): division by zero");
    }
}
