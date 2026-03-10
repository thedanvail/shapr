#include "formats/StlWriter.hpp"

#include <catch2/catch_test_macros.hpp>
#include <sstream>

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

    // Make sure we have 80 0 for the header
    for(size_t idx = 0; idx < 80; ++idx)
    {
        REQUIRE(bytes[idx] == 0);
    }
    REQUIRE(bytes[80] == 0x0C);
    REQUIRE(bytes[81] == 0x00);
    REQUIRE(bytes[82] == 0x00);
    REQUIRE(bytes[83] == 0x00);
}
