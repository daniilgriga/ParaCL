#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "interpreter/context.hpp"

TEST (ContextTest, SetAndGetVar)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    ctx.set_var ("x", 36);
    EXPECT_EQ (ctx.get_var ("x"), 36);

    ctx.set_var ("y", -10);
    EXPECT_EQ (ctx.get_var ("y"), -10);
}

TEST (ContextTest, OverwriteVar)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    ctx.set_var ("x", 1);
    EXPECT_EQ (ctx.get_var ("x"), 1);

    ctx.set_var ("x", 99);
    EXPECT_EQ (ctx.get_var ("x"), 99);
}

TEST (ContextTest, HasVar)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    EXPECT_FALSE (ctx.has_var ("x"));

    ctx.set_var ("x", 5);
    EXPECT_TRUE (ctx.has_var ("x"));
}

TEST (ContextTest, GetUndefinedVarThrows)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    EXPECT_THROW (ctx.get_var ("x"), paracl::RuntimeError);
}

TEST (ContextTest, GetUndefinedVarMessage)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    try
    {
        ctx.get_var ("foo");
    }
    catch (const paracl::RuntimeError& e)
    {
        EXPECT_EQ (std::string (e.what()), "Runtime error: undefined variable 'foo'");
    }
}

TEST (ContextTest, GetVarWithLocationThrows)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    try
    {
        ctx.get_var ("bar", {3, 7});
    }
    catch (const paracl::RuntimeError& e)
    {
        EXPECT_EQ (std::string (e.what()), "Runtime error (3:7): undefined variable 'bar'");
    }
}

TEST (ContextTest, ReadInt)
{
    std::istringstream in ("34");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    EXPECT_EQ (ctx.read_int(), 34);
}

TEST (ContextTest, ReadMultipleInts)
{
    std::istringstream in ("10 20 30");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    EXPECT_EQ (ctx.read_int(), 10);
    EXPECT_EQ (ctx.read_int(), 20);
    EXPECT_EQ (ctx.read_int(), 30);
}

TEST (ContextTest, ReadIntInvalidThrows)
{
    std::istringstream in ("romchik");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    EXPECT_THROW (ctx.read_int(), paracl::RuntimeError);
}

TEST (ContextTest, ReadIntEmptyThrows)
{
    std::istringstream in ("");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    EXPECT_THROW (ctx.read_int(), paracl::RuntimeError);
}

TEST (ContextTest, ReadIntWithLocationThrows)
{
    std::istringstream in ("abc");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    try
    {
        ctx.read_int ({5, 1});
    }
    catch (const paracl::RuntimeError& e)
    {
        EXPECT_EQ (std::string (e.what()), "Runtime error (5:1): failed to read integer from input");
    }
}

TEST (ContextTest, PrintInt)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    ctx.print_int (69);
    EXPECT_EQ (out.str(), "69\n");
}

TEST (ContextTest, PrintMultipleInts)
{
    std::istringstream in;
    std::ostringstream out;
    paracl::Context ctx (in, out);

    ctx.print_int (13);
    ctx.print_int (21);
    ctx.print_int (33);
    EXPECT_EQ (out.str(), "13\n21\n33\n");
}

TEST (ContextTest, ReadAndPrintCombined)
{
    std::istringstream in ("7");
    std::ostringstream out;
    paracl::Context ctx (in, out);

    int val = ctx.read_int();
    ctx.set_var ("x", val);
    ctx.print_int (ctx.get_var ("x"));

    EXPECT_EQ (out.str(), "7\n");
}
