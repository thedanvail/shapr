#include "Exception.hpp"
#include "MeshConverter.hpp"
#include "formats/ObjReader.hpp"
#include "formats/StlWriter.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>

namespace
{
    uint32_t ReadUint32Le(const std::string& aBytes, size_t aOffset)
    {
        const auto b0 = static_cast<uint32_t>(static_cast<uint8_t>(aBytes[aOffset]));
        const auto b1 = static_cast<uint32_t>(static_cast<uint8_t>(aBytes[aOffset + 1]));
        const auto b2 = static_cast<uint32_t>(static_cast<uint8_t>(aBytes[aOffset + 2]));
        const auto b3 = static_cast<uint32_t>(static_cast<uint8_t>(aBytes[aOffset + 3]));
        return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    }

    float ReadFloatLe(const std::string& aBytes, size_t aOffset)
    {
        const uint32_t bits = ReadUint32Le(aBytes, aOffset);
        float value = 0.0f;
        std::memcpy(&value, &bits, sizeof(float));
        return value;
    }
} // namespace

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
    REQUIRE(ReadUint32Le(written, 80) == 12);

    // First triangle v0 comes from original (0,0,0) translated by +1 on x.
    REQUIRE(ReadFloatLe(written, 96) == Catch::Approx(1.0f));
    REQUIRE(ReadFloatLe(written, 100) == Catch::Approx(0.0f));
    REQUIRE(ReadFloatLe(written, 104) == Catch::Approx(0.0f));

    REQUIRE(mesh.vertices[0].x == Catch::Approx(1.0f));
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

TEST_CASE("MeshConverter - Duplicate reader registration throws")
{
    MeshConverter converter;
    converter.RegisterReader(std::make_unique<ObjReader>());
    REQUIRE_THROWS_AS(converter.RegisterReader(std::make_unique<ObjReader>()), DuplicateRegisteredReader);
}

TEST_CASE("MeshConverter - Duplicate writer registration throws")
{
    MeshConverter converter;
    converter.RegisterWriter(std::make_unique<StlWriter>());
    REQUIRE_THROWS_AS(converter.RegisterWriter(std::make_unique<StlWriter>()), DuplicateRegisteredWriter);
}

TEST_CASE("MeshConverter - Case-insensitive format lookup")
{
    MeshConverter converter;
    converter.RegisterReader(std::make_unique<ObjReader>());
    converter.RegisterWriter(std::make_unique<StlWriter>());

    std::ifstream cubefile("test_files/test_cube.obj");
    std::ostringstream out;
    Transform transform;

    const Mesh mesh = converter.Convert(cubefile, "OBJ", out, "StL", transform);
    REQUIRE(mesh.vertices.size() == 8);
    REQUIRE(ReadUint32Le(out.str(), 80) == 12);
}
