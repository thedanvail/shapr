#pragma once

#include "Mesh.hpp"
#include <glm/vec3.hpp>

namespace MeshAnalyzer
{
    [[nodiscard]] bool IsInside(const Mesh& aMesh, glm::vec3 aPoint);
    [[nodiscard]] float SurfaceArea(const Mesh& aMesh);
    [[nodiscard]] float Volume(const Mesh& aMesh);
} // namespace MeshAnalyzer
