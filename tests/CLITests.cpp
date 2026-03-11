#include "CLI.hpp"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
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

    struct CliResult
    {
        int returnCode;
        std::string stdoutText;
        std::string stderrText;
    };

    CliResult RunCli(std::initializer_list<const char*> aArgs)
    {
        std::vector<const char*> argv(aArgs);
        StreamCapture outputCapture(std::cout);
        StreamCapture errorCapture(std::cerr);
        const int rc = CLI::Run((int)argv.size(), argv.data());
        return CliResult{
            .returnCode = rc, .stdoutText = outputCapture.Captured(), .stderrText = errorCapture.Captured()};
    }
} // namespace

TEST_CASE("CLI - invalid scale value returns error")
{
    const CliResult result = RunCli({"shapr", "--input", "test_files/test_cube.obj", "--scale", "banana"});

    REQUIRE(result.returnCode == 1);
    REQUIRE(result.stderrText.find("Invalid scale value") != std::string::npos);
}

TEST_CASE("CLI - invalid rotate value returns error")
{
    const CliResult result = RunCli({"shapr", "--input", "test_files/test_cube.obj", "--rotate", "45,0,1"});

    REQUIRE(result.returnCode == 1);
    REQUIRE(result.stderrText.find("Invalid rotate format") != std::string::npos);
}

TEST_CASE("CLI - zero axis rotate returns error")
{
    const CliResult result = RunCli({"shapr", "--input", "test_files/test_cube.obj", "--rotate", "45,0,0,0"});

    REQUIRE(result.returnCode == 1);
    REQUIRE(result.stderrText.find("Rotation axis must be non-zero") != std::string::npos);
}

TEST_CASE("CLI - invalid inside value returns error")
{
    const CliResult result = RunCli({"shapr", "--input", "test_files/test_cube.obj", "--inside", "banana"});

    REQUIRE(result.returnCode == 1);
    REQUIRE(result.stderrText.find("Invalid vec3 format") != std::string::npos);
}

TEST_CASE("CLI - full pipeline happy path succeeds")
{
    const std::filesystem::path outputPath = std::filesystem::temp_directory_path() / "shapr_cli_happy_path.stl";
    std::filesystem::remove(outputPath);

    const std::string outputString = outputPath.string();
    const CliResult result = RunCli({"shapr",
                                     "--input",
                                     "test_files/test_cube.obj",
                                     "--output",
                                     outputString.c_str(),
                                     "--translate",
                                     "1,0,0",
                                     "--surface-area",
                                     "--volume",
                                     "--inside",
                                     "1.5,0.5,0.5"});

    REQUIRE(result.returnCode == 0);
    REQUIRE(result.stderrText.empty());
    REQUIRE(result.stdoutText.find("Surface Area:") != std::string::npos);
    REQUIRE(result.stdoutText.find("Volume:") != std::string::npos);
    REQUIRE(result.stdoutText.find("Inside: yes") != std::string::npos);
    REQUIRE(std::filesystem::exists(outputPath));
    REQUIRE(std::filesystem::file_size(outputPath) > 0);

    std::filesystem::remove(outputPath);
}

TEST_CASE("CLI - missing input returns error")
{
    const CliResult result = RunCli({"shapr", "--surface-area"});

    REQUIRE(result.returnCode == 1);
    REQUIRE(result.stderrText.find("please provide an input file") != std::string::npos);
}

TEST_CASE("CLI - unknown format extension returns no-reader error")
{
    const std::filesystem::path inputPath = std::filesystem::temp_directory_path() / "shapr_cli_unknown_format.xyz";
    {
        std::ofstream inputFile(inputPath.string());
        inputFile << "v 0 0 0\n";
    }

    const std::string inputString = inputPath.string();
    const CliResult result = RunCli({"shapr", "--input", inputString.c_str()});

    REQUIRE(result.returnCode == 1);
    REQUIRE(result.stderrText.find("No reader registered for format") != std::string::npos);

    std::filesystem::remove(inputPath);
}

TEST_CASE("CLI - output path open failure returns error")
{
    const CliResult result = RunCli({"shapr", "--input", "test_files/test_cube.obj", "--output", "test_files"});

    REQUIRE(result.returnCode == 1);
    REQUIRE(result.stderrText.find("could not open output file") != std::string::npos);
}
