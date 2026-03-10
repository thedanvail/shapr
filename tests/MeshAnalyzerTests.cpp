#include "MeshAnalyzer.hpp"

#include "Mesh.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("MeshAnalyzer - Measure surface area")
{
    const float surfaceArea = MeshAnalyzer::SurfaceArea(TEST_MESH);
    REQUIRE(surfaceArea == Catch::Approx(6.0f));
}

TEST_CASE("MeshAnalyzer - Measure volume")
{
    const float volume = MeshAnalyzer::Volume(TEST_MESH);
    REQUIRE(volume == Catch::Approx(1.0f));
}

TEST_CASE("MeshAnalyzer - Point is inside cube")
{
    REQUIRE(MeshAnalyzer::IsInside(TEST_MESH, glm::vec3{0.5f, 0.5f, 0.5f}));
}

TEST_CASE("MeshAnalyzer - Point is outside cube")
{
    REQUIRE_FALSE(MeshAnalyzer::IsInside(TEST_MESH, glm::vec3{2.0f, 2.0f, 2.0f}));
}

// Points exactly on the surface are undefined behavior.
// Algo assumes the point is strictly inside or outside.
