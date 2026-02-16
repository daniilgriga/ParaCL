#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "interpreter/context.hpp"
#include "syntax/driver.hpp"
#include "syntax/lexer.hpp"

namespace
{
    std::vector<yy::parser::token_type> lex_all (std::string_view source)
    {
        std::istringstream source_stream{std::string (source)};
        yy::Lexer lexer (&source_stream);
        yy::parser::location_type loc;
        lexer.set_context (&loc);

        std::vector<yy::parser::token_type> out;
        while (true)
        {
            const int raw = lexer.yylex ();
            if (raw == 0)
                break;
            out.push_back (static_cast<yy::parser::token_type> (raw));
        }

        return out;
    }

    std::string run_program (std::string_view source,
                             std::string source_name = "<test>",
                             std::string input_data = "")
    {
        std::istringstream source_stream{std::string (source)};
        yy::Lexer lexer (&source_stream);
        yy::Driver driver (&lexer, std::move (source_name));
        driver.parse ();

        const paracl::Stmt* root = driver.root ();
        if (root == nullptr)
            throw std::runtime_error ("parser produced null AST root");

        std::istringstream input_stream (std::move (input_data));
        std::ostringstream output_stream;
        paracl::Context ctx (input_stream, output_stream);
        root->exec (ctx);

        return output_stream.str ();
    }
} // namespace

TEST (SyntaxLexerTest, KeywordsVsIdentifiers)
{
    const auto tokens = lex_all ("if iff print print1 while while2 else else_");

    const std::vector<yy::parser::token_type> expected =
    {
        yy::parser::token_type::IF,
        yy::parser::token_type::VAR,
        yy::parser::token_type::PRINT,
        yy::parser::token_type::VAR,
        yy::parser::token_type::WHILE,
        yy::parser::token_type::VAR,
        yy::parser::token_type::ELSE,
        yy::parser::token_type::VAR,
    };

    EXPECT_EQ (tokens, expected);
}

TEST (SyntaxLexerTest, CommentsAndWhitespaceAreIgnored)
{
    const auto tokens = lex_all (
        "  // comment on line 1\n"
        "\tprint  123 ;   // comment on line 2\n");

    const std::vector<yy::parser::token_type> expected =
    {
        yy::parser::token_type::PRINT,
        yy::parser::token_type::NUMBER,
        yy::parser::token_type::SCOLON,
    };

    EXPECT_EQ (tokens, expected);
}

TEST (SyntaxLexerTest, LeadingZeroProducesErrorToken)
{
    const auto tokens = lex_all ("print 012;");

    ASSERT_GE (tokens.size (), 2u);
    EXPECT_EQ (tokens[0], yy::parser::token_type::PRINT);
    EXPECT_EQ (tokens[1], yy::parser::token_type::YYerror);
}

TEST (SyntaxLexerTest, UnknownCharacterProducesErrorToken)
{
    const auto tokens = lex_all ("print @;");

    ASSERT_GE (tokens.size (), 2u);
    EXPECT_EQ (tokens[0], yy::parser::token_type::PRINT);
    EXPECT_EQ (tokens[1], yy::parser::token_type::YYerror);
}

TEST (SyntaxLexerTest, LogicalOperatorsTokenized)
{
    const auto tokens = lex_all ("print !1; print 1 && 0; print 1 || 0; print 1 ^ 1;");

    const std::vector<yy::parser::token_type> expected =
    {
        yy::parser::token_type::PRINT, yy::parser::token_type::NOT, yy::parser::token_type::NUMBER, yy::parser::token_type::SCOLON,
        yy::parser::token_type::PRINT, yy::parser::token_type::NUMBER, yy::parser::token_type::AND, yy::parser::token_type::NUMBER, yy::parser::token_type::SCOLON,
        yy::parser::token_type::PRINT, yy::parser::token_type::NUMBER, yy::parser::token_type::OR, yy::parser::token_type::NUMBER, yy::parser::token_type::SCOLON,
        yy::parser::token_type::PRINT, yy::parser::token_type::NUMBER, yy::parser::token_type::XOR, yy::parser::token_type::NUMBER, yy::parser::token_type::SCOLON,
    };

    EXPECT_EQ (tokens, expected);
}

TEST (SyntaxParserTest, ParseBuildsRootAst)
{
    std::istringstream source_stream ("print 42;");
    yy::Lexer lexer (&source_stream);
    yy::Driver driver (&lexer, "root_check.paracl");

    EXPECT_NO_THROW (driver.parse ());
    EXPECT_NE (driver.root (), nullptr);
}

TEST (SyntaxParserTest, DanglingElseBindsToNearestIf)
{
    std::istringstream source_stream (
        "if (1)\n"
        "  if (0) print 1;\n"
        "  else print 2;\n");
    yy::Lexer lexer (&source_stream);
    yy::Driver driver (&lexer, "dangling_else.paracl");

    EXPECT_NO_THROW (driver.parse ());
    EXPECT_NE (driver.root (), nullptr);
}

TEST (SyntaxParserTest, SyntaxErrorHasFileAndLocation)
{
    std::istringstream source_stream ("200 + ;\n");
    yy::Lexer lexer (&source_stream);
    yy::Driver driver (&lexer, "bad_input.paracl");

    try
    {
        driver.parse ();
        FAIL () << "expected SyntaxError";
    }
    catch (const paracl::SyntaxError& e)
    {
        const std::string msg = e.what ();
        EXPECT_NE (msg.find ("Syntax error"), std::string::npos);
        EXPECT_NE (msg.find ("bad_input.paracl:1:7"), std::string::npos);
    }
}

TEST (SyntaxLogicalTest, UnaryAndBinaryLogicalOps)
{
    const std::string source =
        "print !0;\n"
        "print !5;\n"
        "print 1 && 0;\n"
        "print 2 && 7;\n"
        "print 0 || 0;\n"
        "print 0 || 9;\n"
        "print 1 ^ 1;\n"
        "print 1 ^ 0;\n";

    EXPECT_EQ (run_program (source), "1\n0\n0\n1\n0\n1\n0\n1\n");
}

TEST (SyntaxLogicalTest, LogicalPrecedence)
{
    const std::string source =
        "print !1 || 1 && 0;\n" // (!1) || (1 && 0) -> 0
        "print !0 && 2 ^ 1;\n"; // (!0) && (2^1) -> 1 && 0 -> 0

    EXPECT_EQ (run_program (source), "0\n0\n");
}
