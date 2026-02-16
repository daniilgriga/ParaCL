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

TEST (UnaryExprTest, LogicalNot)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* zero = arena.make_expr<paracl::IntLiteral> (0);
    auto* five = arena.make_expr<paracl::IntLiteral> (5);

    paracl::UnaryExpr not_zero (paracl::UnOp::Not, zero);
    paracl::UnaryExpr not_five (paracl::UnOp::Not, five);

    EXPECT_EQ (not_zero.eval (ctx), 1);
    EXPECT_EQ (not_five.eval (ctx), 0);
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

TEST (BinaryExprTest, Subtraction)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* lhs = arena.make_expr<paracl::IntLiteral> (13);
    auto* rhs = arena.make_expr<paracl::IntLiteral> (3);

    paracl::BinaryExpr sub (paracl::BinOp::Sub, lhs, rhs);

    EXPECT_EQ (sub.eval (ctx), 10);
}

TEST (BinaryExprTest, Multiplication)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* lhs = arena.make_expr<paracl::IntLiteral> (4);
    auto* rhs = arena.make_expr<paracl::IntLiteral> (5);

    paracl::BinaryExpr mul (paracl::BinOp::Mul, lhs, rhs);

    EXPECT_EQ (mul.eval (ctx), 20);
}

TEST (BinaryExprTest, NestedExpression)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    // (2 + 3) * 12 / 4 = 15
    TestArena arena;
    auto* two = arena.make_expr<paracl::IntLiteral> (2);
    auto* three = arena.make_expr<paracl::IntLiteral> (3);
    auto* sum = arena.make_expr<paracl::BinaryExpr> (paracl::BinOp::Add, two, three);
    auto* twelve = arena.make_expr<paracl::IntLiteral> (12);
    auto* four = arena.make_expr<paracl::IntLiteral> (4);
    auto* div = arena.make_expr<paracl::BinaryExpr> (paracl::BinOp::Div, twelve, four);

    paracl::BinaryExpr mul (paracl::BinOp::Mul, sum, div);

    EXPECT_EQ (mul.eval (ctx), 15);
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

TEST (BinaryExprTest, LogicalAndOrXor)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* zero = arena.make_expr<paracl::IntLiteral> (0);
    auto* two = arena.make_expr<paracl::IntLiteral> (2);
    auto* three = arena.make_expr<paracl::IntLiteral> (3);

    paracl::BinaryExpr and_false (paracl::BinOp::And, zero, two);
    paracl::BinaryExpr and_true (paracl::BinOp::And, two, three);
    paracl::BinaryExpr or_false (paracl::BinOp::Or, zero, zero);
    paracl::BinaryExpr or_true (paracl::BinOp::Or, zero, three);
    paracl::BinaryExpr xor_false (paracl::BinOp::Xor, two, three);
    paracl::BinaryExpr xor_true (paracl::BinOp::Xor, zero, three);

    EXPECT_EQ (and_false.eval (ctx), 0);
    EXPECT_EQ (and_true.eval (ctx), 1);
    EXPECT_EQ (or_false.eval (ctx), 0);
    EXPECT_EQ (or_true.eval (ctx), 1);
    EXPECT_EQ (xor_false.eval (ctx), 0);
    EXPECT_EQ (xor_true.eval (ctx), 1);
}

TEST (BinaryExprTest, LogicalAndShortCircuit)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* zero = arena.make_expr<paracl::IntLiteral> (0);
    auto* ten = arena.make_expr<paracl::IntLiteral> (10);
    auto* div_zero_rhs = arena.make_expr<paracl::BinaryExpr> (
        paracl::BinOp::Div, ten, zero);

    paracl::BinaryExpr and_expr (paracl::BinOp::And, zero, div_zero_rhs);

    EXPECT_NO_THROW ({
        EXPECT_EQ (and_expr.eval (ctx), 0);
    });
}

TEST (BinaryExprTest, LogicalOrShortCircuit)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* one = arena.make_expr<paracl::IntLiteral> (1);
    auto* ten = arena.make_expr<paracl::IntLiteral> (10);
    auto* zero = arena.make_expr<paracl::IntLiteral> (0);
    auto* div_zero_rhs = arena.make_expr<paracl::BinaryExpr> (
        paracl::BinOp::Div, ten, zero);

    paracl::BinaryExpr or_expr (paracl::BinOp::Or, one, div_zero_rhs);

    EXPECT_NO_THROW ({
        EXPECT_EQ (or_expr.eval (ctx), 1);
    });
}

// x = 42;  -> returns 42
TEST (AssignExprTest, AssignsAndReturnsValue)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* val = arena.make_expr<paracl::IntLiteral> (42);

    paracl::AssignExpr assign ("x", val);

    EXPECT_EQ (assign.eval (ctx), 42);
    EXPECT_EQ (ctx.get_var ("x"), 42);
}

// i = 2 + (j = 1 + (k = 1));
// -> k = 1, j = 2, i = 4
TEST (AssignExprTest, NestedAssignment)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* one = arena.make_expr<paracl::IntLiteral> (1);
    auto* k_assign = arena.make_expr<paracl::AssignExpr> ("k", one);

    auto* one2 = arena.make_expr<paracl::IntLiteral> (1);
    auto* j_rhs = arena.make_expr<paracl::BinaryExpr> (
        paracl::BinOp::Add, one2, k_assign);
    auto* j_assign = arena.make_expr<paracl::AssignExpr> ("j", j_rhs);

    auto* two = arena.make_expr<paracl::IntLiteral> (2);
    auto* i_rhs = arena.make_expr<paracl::BinaryExpr> (
        paracl::BinOp::Add, two, j_assign);

    paracl::AssignExpr i_assign ("i", i_rhs);

    EXPECT_EQ (i_assign.eval (ctx), 4);
    EXPECT_EQ (ctx.get_var ("i"), 4);
    EXPECT_EQ (ctx.get_var ("j"), 2);
    EXPECT_EQ (ctx.get_var ("k"), 1);
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

// if (1) { print 89; }
TEST (IfStmtTest, ConditionTrue)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* cond = arena.make_expr<paracl::IntLiteral> (1);
    auto* val = arena.make_expr<paracl::IntLiteral> (89);
    auto* then_branch = arena.make_stmt<paracl::PrintStmt> (val);

    paracl::IfStmt if_stmt (cond, then_branch);
    if_stmt.exec (ctx);

    EXPECT_EQ (out.str(), "89\n");
}

// if (0)
// { print 1; }
// else
// { print 2; }
TEST (IfStmtTest, ConditionFalseWithElse)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* cond = arena.make_expr<paracl::IntLiteral> (0);
    auto* then_val = arena.make_expr<paracl::IntLiteral> (1);
    auto* else_val = arena.make_expr<paracl::IntLiteral> (2);
    auto* then_branch = arena.make_stmt<paracl::PrintStmt> (then_val);
    auto* else_branch = arena.make_stmt<paracl::PrintStmt> (else_val);

    paracl::IfStmt if_stmt (cond, then_branch, else_branch);
    if_stmt.exec (ctx);

    EXPECT_EQ (out.str(), "2\n");
}

// if (0) { print 42; }
TEST (IfStmtTest, ConditionFalseNoElse)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* cond = arena.make_expr<paracl::IntLiteral> (0);
    auto* val = arena.make_expr<paracl::IntLiteral> (42);
    auto* then_branch = arena.make_stmt<paracl::PrintStmt> (val);

    paracl::IfStmt if_stmt (cond, then_branch);
    if_stmt.exec (ctx);

    EXPECT_EQ (out.str(), "");
}

// if (-5) { print 99; }
TEST (IfStmtTest, NonZeroIsTruthy)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* cond = arena.make_expr<paracl::IntLiteral> (-5);
    auto* val = arena.make_expr<paracl::IntLiteral> (99);
    auto* then_branch = arena.make_stmt<paracl::PrintStmt> (val);

    paracl::IfStmt if_stmt (cond, then_branch);
    if_stmt.exec (ctx);

    EXPECT_EQ (out.str(), "99\n");
}

// while (0) { print 89; }
TEST (WhileStmtTest, ConditionFalseImmediately)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;
    auto* cond = arena.make_expr<paracl::IntLiteral> (0);
    auto* val = arena.make_expr<paracl::IntLiteral> (89);
    auto* body = arena.make_stmt<paracl::PrintStmt> (val);

    paracl::WhileStmt while_stmt (cond, body);
    while_stmt.exec (ctx);

    EXPECT_EQ (out.str(), "");
}

// x = 3;
// while (x)
// {
//     print x;
//     x = x - 1;
// }
// output: 3, 2, 1
TEST (WhileStmtTest, CountDown)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;

    // condition: x
    auto* cond = arena.make_expr<paracl::VarRef> ("x");

    // body: print x; x = x - 1;
    auto* var_x = arena.make_expr<paracl::VarRef> ("x");
    auto* one = arena.make_expr<paracl::IntLiteral> (1);
    auto* x_minus_1 = arena.make_expr<paracl::BinaryExpr> (
        paracl::BinOp::Sub, var_x, one);

    auto* print_x = arena.make_stmt<paracl::PrintStmt> (
        arena.make_expr<paracl::VarRef> ("x"));
    auto* assign_x = arena.make_stmt<paracl::AssignStmt> ("x", x_minus_1);

    auto* body = arena.make_stmt<paracl::BlockStmt> (
        std::vector<const paracl::Stmt*>{print_x, assign_x});

    // setup: x = 3
    ctx.set_var ("x", 3);

    paracl::WhileStmt while_stmt (cond, body);
    while_stmt.exec (ctx);

    EXPECT_EQ (out.str(), "3\n2\n1\n");
    EXPECT_EQ (ctx.get_var ("x"), 0);
}

// if (3 > 2)
// { x = 10; }
// else
// { x = 20; }
// print x;
TEST (IntegrationTest, IfWithComparison)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    TestArena arena;

    auto* three = arena.make_expr<paracl::IntLiteral> (3);
    auto* two = arena.make_expr<paracl::IntLiteral> (2);
    auto* cond = arena.make_expr<paracl::BinaryExpr> (
        paracl::BinOp::Gt, three, two);

    auto* ten = arena.make_expr<paracl::IntLiteral> (10);
    auto* twenty = arena.make_expr<paracl::IntLiteral> (20);
    auto* then_branch = arena.make_stmt<paracl::AssignStmt> ("x", ten);
    auto* else_branch = arena.make_stmt<paracl::AssignStmt> ("x", twenty);

    auto* if_stmt = arena.make_stmt<paracl::IfStmt> (cond, then_branch, else_branch);
    auto* print_x = arena.make_stmt<paracl::PrintStmt> (
        arena.make_expr<paracl::VarRef> ("x"));

    paracl::BlockStmt program ({if_stmt, print_x});
    program.exec (ctx);

    EXPECT_EQ (out.str(), "10\n");
}
