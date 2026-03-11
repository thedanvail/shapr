#include "formats/StlWriter.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <cstring>
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

    uint16_t ReadUint16Le(const std::string& aBytes, size_t aOffset)
    {
        const auto b0 = static_cast<uint16_t>(static_cast<uint8_t>(aBytes[aOffset]));
        const auto b1 = static_cast<uint16_t>(static_cast<uint8_t>(aBytes[aOffset + 1]));
        return static_cast<uint16_t>(b0 | (b1 << 8));
    }

    float ReadFloatLe(const std::string& aBytes, size_t aOffset)
    {
        const uint32_t bits = ReadUint32Le(aBytes, aOffset);
        float value = 0.0f;
        std::memcpy(&value, &bits, sizeof(float));
        return value;
    }
} // namespace

static const Mesh TEST_MESH = Mesh{.vertices = std::vector<glm::vec3>{glm::vec3{0.0f, 0.0f, 0.0f},
                                                                      glm::vec3{1.0f, 0.0f, 0.0f},
                                                                      glm::vec3{1.0f, 1.0f, 0.0f},
                                                                      glm::vec3{0.0f, 1.0f, 0.0f},
                                                                      glm::vec3{0.0f, 0.0f, 1.0f},
                                                                      glm::vec3{1.0f, 0.0f, 1.0f},
                                                                      glm::vec3{1.0f, 1.0f, 1.0f},
                                                                      glm::vec3{0.0f, 1.0f, 1.0f}},
                                   .triangles = std::vector<std::array<uint32_t, 3>>{
                                       std::array<uint32_t, 3>{0, 2, 1},
                                       std::array<uint32_t, 3>{0, 3, 2},
                                       std::array<uint32_t, 3>{4, 5, 6},
                                       std::array<uint32_t, 3>{4, 6, 7},
                                       std::array<uint32_t, 3>{0, 1, 5},
                                       std::array<uint32_t, 3>{0, 5, 4},
                                       std::array<uint32_t, 3>{3, 6, 2},
                                       std::array<uint32_t, 3>{3, 7, 6},
                                       std::array<uint32_t, 3>{0, 4, 7},
                                       std::array<uint32_t, 3>{0, 7, 3},
                                       std::array<uint32_t, 3>{1, 2, 6},
                                       std::array<uint32_t, 3>{1, 6, 5},
                                   }};

TEST_CASE("StlWriter - File is written correctly")
{
    std::ostringstream out;
    StlWriter writer;
    writer.Write(out, TEST_MESH);
    std::string bytes = out.str();

    // Do we have the expected file size?
    REQUIRE(bytes.size() == 684);

    // Make sure we have 80 0 for the header.
    for(size_t idx = 0; idx < 80; ++idx)
    {
        REQUIRE(bytes[idx] == 0);
    }

    // Verify triangle count.
    REQUIRE(ReadUint32Le(bytes, 80) == 12);

    // Verify first triangle payload (normal + vertices + attr bytes).
    REQUIRE(ReadFloatLe(bytes, 84) == Catch::Approx(0.0f));
    REQUIRE(ReadFloatLe(bytes, 88) == Catch::Approx(0.0f));
    REQUIRE(ReadFloatLe(bytes, 92) == Catch::Approx(-1.0f));

    REQUIRE(ReadFloatLe(bytes, 96) == Catch::Approx(0.0f));
    REQUIRE(ReadFloatLe(bytes, 100) == Catch::Approx(0.0f));
    REQUIRE(ReadFloatLe(bytes, 104) == Catch::Approx(0.0f));

    REQUIRE(ReadFloatLe(bytes, 108) == Catch::Approx(1.0f));
    REQUIRE(ReadFloatLe(bytes, 112) == Catch::Approx(1.0f));
    REQUIRE(ReadFloatLe(bytes, 116) == Catch::Approx(0.0f));

    REQUIRE(ReadFloatLe(bytes, 120) == Catch::Approx(1.0f));
    REQUIRE(ReadFloatLe(bytes, 124) == Catch::Approx(0.0f));
    REQUIRE(ReadFloatLe(bytes, 128) == Catch::Approx(0.0f));

    REQUIRE(ReadUint16Le(bytes, 132) == 0);
}
