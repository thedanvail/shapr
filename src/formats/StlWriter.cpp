#include "formats/StlWriter.hpp"
#include "Exception.hpp"

#include <bit>
#include <glm/geometric.hpp>

namespace
{
    constexpr bool kSysIsLittleEndian = std::endian::native == std::endian::little;

    template <typename T> T ByteSwap(T aValue)
    {
        static_assert(std::is_arithmetic_v<T>);
        T result;
        auto src = reinterpret_cast<const char*>(&aValue);
        auto dst = reinterpret_cast<char*>(&result);
        for(size_t i = 0; i < sizeof(T); ++i)
        {
            dst[i] = src[sizeof(T) - 1 - i];
        }
        return result;
    }

    template <typename T> T ToLittleEndian(T aValue)
    {
        if constexpr(kSysIsLittleEndian)
        {
            return aValue;
        }
        return ByteSwap(aValue);
    }

    void WriteFloat(std::ostream& aStream, float aValue)
    {
        const float v = ToLittleEndian(aValue);
        aStream.write(reinterpret_cast<const char*>(&v), sizeof(float));
    }

    void WriteVec3(std::ostream& aStream, glm::vec3 aVec)
    {
        WriteFloat(aStream, aVec.x);
        WriteFloat(aStream, aVec.y);
        WriteFloat(aStream, aVec.z);
    }

    void WriteUint32(std::ostream& aStream, uint32_t aValue)
    {
        const uint32_t v = ToLittleEndian(aValue);
        aStream.write(reinterpret_cast<const char*>(&v), sizeof(uint32_t));
    }

    void WriteUint16(std::ostream& aStream, uint16_t aValue)
    {
        const uint16_t v = ToLittleEndian(aValue);
        aStream.write(reinterpret_cast<const char*>(&v), sizeof(uint16_t));
    }
} // namespace

void StlWriter::Write(std::ostream& aStream, const Mesh& aMesh) const
{
    // If we fail to write, just exit immediately. No point in
    // trying to write if we know we'll fail and/or we're working
    // with a corrupted file.
    if(!aStream.good())
    {
        throw StlWriteError{"Failed to write STL output."};
    }

    // pad 80 byte header with 0
    constexpr size_t kHeaderSize = 80;
    char header[kHeaderSize] = {};
    aStream.write(header, kHeaderSize);

    WriteUint32(aStream, static_cast<uint32_t>(aMesh.triangles.size()));

    for(const auto& tri : aMesh.triangles)
    {
        const glm::vec3& v0 = aMesh.vertices[tri[0]];
        const glm::vec3& v1 = aMesh.vertices[tri[1]];
        const glm::vec3& v2 = aMesh.vertices[tri[2]];

        // Compute per-face normals (`Mesh` stores per-vertex normals)
        const glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        WriteVec3(aStream, normal);
        WriteVec3(aStream, v0);
        WriteVec3(aStream, v1);
        WriteVec3(aStream, v2);

        // attribute byte count, always 0
        const uint16_t attr = 0;
        WriteUint16(aStream, attr);
    }
}
