#include "MeshAnalyzer.hpp"

#include <cmath>

#include <glm/geometric.hpp>

namespace
{
    /*
     * Choosing Möller–Trumbore for the algorithm as we don't need to precompute
     * the triangle's normal or plane equation stored ahead of time.
     * Additionally, we can exit early if we know it's going to be parallel
     * to the triangle or outside of its bounds.
     */
    bool RayIntersectsTriangle(glm::vec3 aOrigin, glm::vec3 aDirection, glm::vec3 aV0, glm::vec3 aV1, glm::vec3 aV2)
    {
        constexpr float kEpsilon = 1e-8f;

        const glm::vec3 e1 = aV1 - aV0;
        const glm::vec3 e2 = aV2 - aV0;
        const glm::vec3 h = glm::cross(aDirection, e2);
        const float det = glm::dot(e1, h);

        if(std::abs(det) < kEpsilon)
        {
            // parallel to triangle
            return false;
        }

        const float invDet = 1.0f / det;
        const glm::vec3 T = aOrigin - aV0;
        const float u = invDet * glm::dot(T, h);

        if(u < 0.0f || u > 1.0f)
        {
            // outside triangle
            return false;
        }

        const glm::vec3 q = glm::cross(T, e1);
        const float v = invDet * glm::dot(aDirection, q);

        if(v < 0.0f || u + v > 1.0f)
        {
            // outside triangle
            return false;
        }

        // intersection ahead of ray origin
        //
        // `invDet * glm::dot(e2, q)` equates to `t`, but we can skip the allocation
        // by simply returning the equality expression. The compiler would skip the allocation,
        // but being explicit doesn't hurt.
        return invDet * glm::dot(e2, q) > kEpsilon;
    }

    int CountIntersections(const Mesh& aMesh, glm::vec3 aOrigin, glm::vec3 aDirection)
    {
        int count = 0;
        for(const auto& triangle : aMesh.triangles)
        {
            if(RayIntersectsTriangle(aOrigin,
                                     aDirection,
                                     aMesh.vertices[triangle[0]],
                                     aMesh.vertices[triangle[1]],
                                     aMesh.vertices[triangle[2]]))
            {
                count++;
            }
        }
        return count;
    }
} // namespace

/*
 * When I was writing tests for this project, I found an issue where
 * the ray I was tracing was intersecting with the edge of two triangles,
 * effectively counting it twice. Originally, I was going to use a ray that
 * was slightly unaligned with the axis, but with a large enough number of triangles,
 * that would not be sufficient, so I went with a majority-vote style counting alongside the
 * slightly askew ray (but not on the diagonal, which is why some of these values are weird).
 * If two or more ray traces agree on position, then it is counted as a valid result.
 */
bool MeshAnalyzer::IsInside(const Mesh& aMesh, glm::vec3 aPoint)
{
    int collisions = 0;
    collisions += CountIntersections(aMesh, aPoint, {1.0f, 1e-4f, 2e-4f}) & 1;
    collisions += CountIntersections(aMesh, aPoint, {3e-4f, 1.0f, 1e-4f}) & 1;
    collisions += CountIntersections(aMesh, aPoint, {2e-4f, 3e-4f, 1.0f}) & 1;
    return collisions >= 2;
}

float MeshAnalyzer::SurfaceArea(const Mesh& aMesh)
{
    float surfaceArea = 0.0f;
    for(const std::array<uint32_t, 3>& triangle : aMesh.triangles)
    {
        const glm::vec3& v0 = aMesh.vertices[triangle[0]];
        const glm::vec3& v1 = aMesh.vertices[triangle[1]];
        const glm::vec3& v2 = aMesh.vertices[triangle[2]];

        const auto edge1 = v1 - v0;
        const auto edge2 = v2 - v0;

        surfaceArea += glm::length(glm::cross(edge1, edge2)) / 2.0f;
    }
    return surfaceArea;
}

float MeshAnalyzer::Volume(const Mesh& aMesh)
{
    float volume = 0.0f;
    for(const auto& triangle : aMesh.triangles)
    {
        const glm::vec3& v0 = aMesh.vertices[triangle[0]];
        const glm::vec3& v1 = aMesh.vertices[triangle[1]];
        const glm::vec3& v2 = aMesh.vertices[triangle[2]];

        volume += glm::dot(v0, glm::cross(v1, v2)) / 6.0f;
    }
    return std::abs(volume);
}
