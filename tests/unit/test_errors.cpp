#include <gtest/gtest.h>

#include <string>
#include <stdexcept>

#include "errors/errors.hpp"

TEST (ErrorsTest, FormatLocationUnknown)
{
    EXPECT_EQ (paracl::format_location ({0, 0}), "");
    EXPECT_EQ (paracl::format_location ({0, 10}), "");
    EXPECT_EQ (paracl::format_location ({-1, 10}), "");
    EXPECT_EQ (paracl::format_location ({-100, -5}), "");
}

TEST (ErrorsTest, FormatLocationLineOnly)
{
    EXPECT_EQ (paracl::format_location ({42, 0}), " (42)");
    EXPECT_EQ (paracl::format_location ({42, -3}), " (42)");
    EXPECT_EQ (paracl::format_location ({1, 0}), " (1)");
}

TEST (ErrorsTest, FormatLocationLineAndColumn)
{
    EXPECT_EQ (paracl::format_location ({42, 15}), " (42:15)");
    EXPECT_EQ (paracl::format_location ({1, 1}), " (1:1)");
}

TEST (ErrorsTest, SyntaxErrorThrows)
{
    EXPECT_THROW (throw paracl::SyntaxError ("unexpected token"), paracl::SyntaxError);
    EXPECT_THROW (throw paracl::SyntaxError ({5, 12}, "unexpected token ';'"), paracl::SyntaxError);
}

TEST (ErrorsTest, SyntaxErrorMessageNoLocation)
{
    try
    {
        throw paracl::SyntaxError ("unexpected token");
    }
    catch (const paracl::SyntaxError& e)
    {
        EXPECT_EQ (std::string (e.what()), "Syntax error: unexpected token");
    }
}

TEST (ErrorsTest, SyntaxErrorMessageWithLocation)
{
    try
    {
        throw paracl::SyntaxError ({5, 12}, "unexpected token ';'");
    }
    catch (const paracl::SyntaxError& e)
    {
        EXPECT_EQ (std::string (e.what()), "Syntax error (5:12): unexpected token ';'");
    }
}

TEST (ErrorsTest, RuntimeErrorThrows)
{
    EXPECT_THROW (throw paracl::RuntimeError ("division by zero"), paracl::RuntimeError);
    EXPECT_THROW (throw paracl::RuntimeError ({10, 8}, "undefined variable 'x'"), paracl::RuntimeError);
}

TEST (ErrorsTest, RuntimeErrorMessageNoLocation)
{
    try
    {
        throw paracl::RuntimeError ("division by zero");
    }
    catch (const paracl::RuntimeError& e)
    {
        EXPECT_EQ (std::string (e.what()), "Runtime error: division by zero");
    }
}

TEST (ErrorsTest, RuntimeErrorMessageWithLocation)
{
    try
    {
        throw paracl::RuntimeError ({10, 8}, "undefined variable 'x'");
    }
    catch (const paracl::RuntimeError& e)
    {
        EXPECT_EQ (std::string (e.what()), "Runtime error (10:8): undefined variable 'x'");
    }
}

TEST (ErrorsTest, BaseClassCatchesDerivedNoSlicing)
{
    // catch SyntaxError as ParaCLError
    try
    {
        throw paracl::SyntaxError ({1, 2}, "test");
    }
    catch (const paracl::ParaCLError& e) // no slicing
    {
        EXPECT_FALSE (std::string (e.what()).empty());
        EXPECT_EQ (std::string (e.what()), "Syntax error (1:2): test");
    }

    // catch RuntimeError as ParaCLError
    try
    {
        throw paracl::RuntimeError ({3, 4}, "test");
    }
    catch (const paracl::ParaCLError& e) // no slicing
    {
        EXPECT_FALSE (std::string (e.what()).empty());
        EXPECT_EQ (std::string (e.what()), "Runtime error (3:4): test");
    }
}

TEST (ErrorsTest, StdRuntimeErrorCatchesParaCL)
{
    try
    {
        throw paracl::SyntaxError ({1, 1}, "test");
    }
    catch (const std::runtime_error& e)
    {
        EXPECT_FALSE (std::string (e.what()).empty());
        EXPECT_EQ (std::string (e.what()), "Syntax error (1:1): test");
    }
}
