#include "formats/ObjReader.hpp"
#include "Exception.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>
#include <tuple>
#include <unordered_map>

namespace
{
    /*
     * Hashing a tuple of three ints for use as an unordered_map key.
     * Uses the boost::hash_combine technique (without bringing in the dependency):
     * XOR each hash with the next, shifted and offset by 0x9e3779b9 (the golden ratio in hex).
     * This distributes bits evenly and avoids collisions between permutations
     * like (1,0,0) and (0,1,0) that standard XOR would treat as identical.
     */
    struct IndexHash
    {
        size_t operator()(const std::tuple<int, int, int>& t) const
        {
            size_t h = std::hash<int>{}(std::get<0>(t));
            h ^= std::hash<int>{}(std::get<1>(t)) + 0x9E3779B9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(std::get<2>(t)) + 0x9E3779B9 + (h << 6) + (h >> 2);
            return h;
        }
    };
} // namespace

Mesh ObjReader::Read(std::istream& aStream) const
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &aStream))
    {
        throw ObjParseError{"Failed to parse OBJ: " + err};
    }

    if(!warn.empty())
    {
        std::cerr << "OBJ warning: " << warn << std::endl;
    }

    Mesh mesh;
    if(!attrib.normals.empty())
    {
        mesh.normals = std::vector<glm::vec3>{};
    }

    if(!attrib.texcoords.empty())
    {
        mesh.texcoords = std::vector<glm::vec2>{};
    }
    std::unordered_map<std::tuple<int, int, int>, uint32_t, IndexHash> indexMap;
    indexMap.reserve(attrib.vertices.size() / 3);

    // Pre-reserve space ahead of time. Slight overhead, but we more than
    // make up for that in skipping reallocations which are O(log n)
    // and our triangle processing is O(n).
    size_t totalTris = 0;
    for(const auto& shape : shapes)
    {
        for(const auto nv : shape.mesh.num_face_vertices)
        {
            totalTris += nv - 2;
        }
    }
    mesh.triangles.reserve(totalTris);

    // We can't know the exact num of vertices due to deduplication, but
    // this is a reasonable upper bound.
    mesh.vertices.reserve(attrib.vertices.size() / 3);

    /*
     * This loop looks a lot scarier than it actually is.
     * 1. Looping over shapes:
     * Each obj file can have multiple shapes, and we need to load all of them.
     *
     * 2. Looping over faces:
     * Necessary to visit and process every face.
     *
     * 3. Fan triangulation:
     * Needed to handle n-gons.
     *
     * 4. Corners:
     * Runs exactly three times, compiler will unroll this loop.
     */
    for(const auto& shape : shapes)
    {
        size_t indexOffset = 0;
        for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
        {
            const int faceVerts = shape.mesh.num_face_vertices[f];

            for(int i = 1; i < faceVerts - 1; ++i)
            {
                std::array<uint32_t, 3> tri;
                for(int corner = 0; corner < 3; ++corner)
                {
                    const int fanIndex = (corner == 0) ? 0 : (corner == 1) ? i : i + 1;
                    const auto& idx = shape.mesh.indices[indexOffset + fanIndex];

                    // Discovered during tests that tinyobjloader will not validate any indices,
                    // which makes sense, but today I learned. Bounds check!
                    if(idx.vertex_index < 0 || (idx.vertex_index * 3 + 2) >= (int)attrib.vertices.size())
                    {
                        throw ObjParseError{"Vertex index out of bounds: " + std::to_string(idx.vertex_index)};
                    }

                    const auto key = std::make_tuple(idx.vertex_index, idx.normal_index, idx.texcoord_index);
                    const auto [it, inserted] = indexMap.emplace(key, (uint32_t)mesh.vertices.size());

                    if(inserted)
                    {
                        const int vi = idx.vertex_index * 3;
                        mesh.vertices.push_back(
                            {attrib.vertices[vi], attrib.vertices[vi + 1], attrib.vertices[vi + 2]});

                        if(mesh.normals && idx.normal_index >= 0)
                        {
                            const int ni = idx.normal_index * 3;
                            mesh.normals->push_back(
                                {attrib.normals[ni], attrib.normals[ni + 1], attrib.normals[ni + 2]});
                        }

                        if(mesh.texcoords && idx.texcoord_index >= 0)
                        {
                            const int ti = idx.texcoord_index * 2;
                            mesh.texcoords->push_back({attrib.texcoords[ti], attrib.texcoords[ti + 1]});
                        }
                    }

                    tri[corner] = it->second;
                }

                mesh.triangles.push_back(tri);
            }

            indexOffset += faceVerts;
        }
    }

    return mesh;
}
