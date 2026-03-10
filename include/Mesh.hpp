#pragma once

#include <array>
#include <optional>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Mesh
{
    std::vector<glm::vec3> vertices;

    /*
     * The choice to use `uint32_t` is that binary STL is fixed-width 32 bit,
     * so there's no conversion necessary when reading/writing.
     */
    std::vector<std::array<uint32_t, 3>> triangles;

    /*
     * The following two fields are optionals to convey
     * that a mesh may not have normals or text coordinates.
     */
    std::optional<std::vector<glm::vec3>> normals;
    std::optional<std::vector<glm::vec2>> texcoords;
};
