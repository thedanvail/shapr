#include "ArgParser.hpp"

#include <vector>

#include <catch2/catch_test_macros.hpp>

static void InitArgs(std::initializer_list<const char*> aArgs)
{
    ArgParser::Reset();
    std::vector<const char*> argv(aArgs);
    ArgParser::Init((int)argv.size(), argv.data());
}

TEST_CASE("ArgParser - no arguments")
{
    InitArgs({"shapr"});
    auto& parser = ArgParser::GetInstance();

    REQUIRE_FALSE(parser.GetArgValue("input").has_value());
    REQUIRE_FALSE(parser.HasFlag("surface-area"));
}

TEST_CASE("ArgParser - key-value argument")
{
    InitArgs({"shapr", "--input", "model.obj"});
    auto& parser = ArgParser::GetInstance();

    REQUIRE(parser.GetArgValue("input").has_value());
    REQUIRE(parser.GetArgValue("input").value() == "model.obj");
}

TEST_CASE("ArgParser - flag argument")
{
    InitArgs({"shapr", "--surface-area"});
    auto& parser = ArgParser::GetInstance();

    REQUIRE(parser.HasFlag("surface-area"));
    REQUIRE_FALSE(parser.GetArgValue("surface-area").has_value());
}

TEST_CASE("ArgParser - unknown key returns nullopt")
{
    InitArgs({"shapr"});
    auto& parser = ArgParser::GetInstance();

    REQUIRE_FALSE(parser.GetArgValue("nonexistent").has_value());
    REQUIRE_FALSE(parser.HasFlag("nonexistent"));
}

TEST_CASE("ArgParser - arg without -- prefix is ignored")
{
    InitArgs({"shapr", "input", "model.obj"});
    auto& parser = ArgParser::GetInstance();

    REQUIRE_FALSE(parser.GetArgValue("input").has_value());
}

TEST_CASE("ArgParser - flag is not retrievable as key-value")
{
    InitArgs({"shapr", "--surface-area"});
    auto& parser = ArgParser::GetInstance();

    REQUIRE_FALSE(parser.GetArgValue("surface-area").has_value());
}

TEST_CASE("ArgParser - multiple args parsed correctly")
{
    InitArgs({"shapr", "--input", "model.obj", "--surface-area", "--volume"});
    auto& parser = ArgParser::GetInstance();

    REQUIRE(parser.GetArgValue("input").value() == "model.obj");
    REQUIRE(parser.HasFlag("surface-area"));
    REQUIRE(parser.HasFlag("volume"));
}
