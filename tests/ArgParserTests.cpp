#include "ArgParser.hpp"

#include <vector>

#include <catch2/catch_test_macros.hpp>

static ArgParser MakeParser(std::initializer_list<const char*> aArgs)
{
    std::vector<const char*> argv(aArgs);
    return ArgParser((int)argv.size(), argv.data());
}

TEST_CASE("ArgParser - no arguments")
{
    auto parser = MakeParser({"shapr"});

    REQUIRE_FALSE(parser.GetArgValue("input").has_value());
    REQUIRE_FALSE(parser.HasFlag("surface-area"));
}

TEST_CASE("ArgParser - key-value argument")
{
    auto parser = MakeParser({"shapr", "--input", "model.obj"});

    REQUIRE(parser.GetArgValue("input").has_value());
    REQUIRE(parser.GetArgValue("input").value() == "model.obj");
}

TEST_CASE("ArgParser - flag argument")
{
    auto parser = MakeParser({"shapr", "--surface-area"});

    REQUIRE(parser.HasFlag("surface-area"));
    REQUIRE_FALSE(parser.GetArgValue("surface-area").has_value());
}

TEST_CASE("ArgParser - unknown key returns nullopt")
{
    auto parser = MakeParser({"shapr"});

    REQUIRE_FALSE(parser.GetArgValue("nonexistent").has_value());
    REQUIRE_FALSE(parser.HasFlag("nonexistent"));
}

TEST_CASE("ArgParser - arg without -- prefix is ignored")
{
    auto parser = MakeParser({"shapr", "input", "model.obj"});

    REQUIRE_FALSE(parser.GetArgValue("input").has_value());
}

TEST_CASE("ArgParser - flag is not retrievable as key-value")
{
    auto parser = MakeParser({"shapr", "--surface-area"});

    REQUIRE_FALSE(parser.GetArgValue("surface-area").has_value());
}

TEST_CASE("ArgParser - multiple args parsed correctly")
{
    auto parser = MakeParser({"shapr", "--input", "model.obj", "--surface-area", "--volume"});

    REQUIRE(parser.GetArgValue("input").value() == "model.obj");
    REQUIRE(parser.HasFlag("surface-area"));
    REQUIRE(parser.HasFlag("volume"));
}
