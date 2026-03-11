#include "CLI.hpp"

#include <catch2/catch_test_macros.hpp>

#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    struct StreamCapture
    {
        explicit StreamCapture(std::ostream& aStream)
            : m_stream(aStream),
              m_originalBuffer(aStream.rdbuf(m_captured.rdbuf()))
        {
        }

        ~StreamCapture() { m_stream.rdbuf(m_originalBuffer); }

        [[nodiscard]] std::string Captured() const { return m_captured.str(); }

    private:
        std::ostream& m_stream;
        std::ostringstream m_captured;
        std::streambuf* m_originalBuffer;
    };

    int RunCli(std::initializer_list<const char*> aArgs, std::string& aCapturedErr)
    {
        std::vector<const char*> argv(aArgs);
        StreamCapture errorCapture(std::cerr);
        const int rc = CLI::Run((int)argv.size(), argv.data());
        aCapturedErr = errorCapture.Captured();
        return rc;
    }
} // namespace

TEST_CASE("CLI - invalid scale value returns error")
{
    std::string errorOutput;
    const int rc = RunCli({"shapr", "--input", "test_files/test_cube.obj", "--scale", "banana"}, errorOutput);

    REQUIRE(rc == 1);
    REQUIRE(errorOutput.find("Invalid scale value") != std::string::npos);
}

TEST_CASE("CLI - invalid rotate value returns error")
{
    std::string errorOutput;
    const int rc = RunCli({"shapr", "--input", "test_files/test_cube.obj", "--rotate", "45,0,1"}, errorOutput);

    REQUIRE(rc == 1);
    REQUIRE(errorOutput.find("Invalid rotate format") != std::string::npos);
}

TEST_CASE("CLI - zero axis rotate returns error")
{
    std::string errorOutput;
    const int rc = RunCli({"shapr", "--input", "test_files/test_cube.obj", "--rotate", "45,0,0,0"}, errorOutput);

    REQUIRE(rc == 1);
    REQUIRE(errorOutput.find("Rotation axis must be non-zero") != std::string::npos);
}

TEST_CASE("CLI - invalid inside value returns error")
{
    std::string errorOutput;
    const int rc = RunCli({"shapr", "--input", "test_files/test_cube.obj", "--inside", "banana"}, errorOutput);

    REQUIRE(rc == 1);
    REQUIRE(errorOutput.find("Invalid vec3 format") != std::string::npos);
}
