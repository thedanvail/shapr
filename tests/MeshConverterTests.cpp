#include "Exception.hpp"
#include "MeshConverter.hpp"
#include "formats/ObjReader.hpp"
#include "formats/StlWriter.hpp"

#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <sstream>

TEST_CASE("MeshConverter - Correctly convert test obj")
{
    MeshConverter converter;
    converter.RegisterReader(std::make_unique<ObjReader>());
    converter.RegisterWriter(std::make_unique<StlWriter>());
    std::ifstream cubefile("test_files/test_cube.obj");
    std::ostringstream out;
    Transform transform;
    transform.Translate({1.0f, 0.0f, 0.0f});
    Mesh mesh = converter.Convert(cubefile, "obj", out, "stl", transform);

    const std::string written = out.str();
    REQUIRE(written.size() == 684);
}

TEST_CASE("MeshConverter - Unknown reader")
{
    MeshConverter converter;
    converter.RegisterReader(std::make_unique<ObjReader>());
    converter.RegisterWriter(std::make_unique<StlWriter>());
    std::ifstream cubefile("test_files/test_cube.obj");
    std::ostringstream out;
    Transform transform;
    transform.Translate({1.0f, 0.0f, 0.0f});
    REQUIRE_THROWS_AS(converter.Convert(cubefile, "yeet", out, "stl", transform), NoRegisteredReader);
}

TEST_CASE("MeshConverter - Unknown writer")
{
    MeshConverter converter;
    converter.RegisterReader(std::make_unique<ObjReader>());
    converter.RegisterWriter(std::make_unique<StlWriter>());
    std::ifstream cubefile("test_files/test_cube.obj");
    std::ostringstream out;
    Transform transform;
    transform.Translate({1.0f, 0.0f, 0.0f});
    REQUIRE_THROWS_AS(converter.Convert(cubefile, "obj", out, "yeet", transform), NoRegisteredWriter);
}
