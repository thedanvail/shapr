#include "MeshConverter.hpp"
#include "Exception.hpp"

#include <cctype>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace
{
    std::string NormalizeFormat(std::string_view aFormat)
    {
        std::string normalized;
        normalized.reserve(aFormat.size());
        for(const unsigned char c : aFormat)
        {
            normalized.push_back(static_cast<char>(std::tolower(c)));
        }
        return normalized;
    }
} // namespace

void MeshConverter::RegisterReader(std::unique_ptr<IMeshReader> aReader)
{
    const std::string format = NormalizeFormat(aReader->GetFormatName());
    if(m_readers.contains(format))
    {
        throw DuplicateRegisteredReader{"Reader already registered for format: " + format};
    }
    m_readers.emplace(format, std::move(aReader));
}

void MeshConverter::RegisterWriter(std::unique_ptr<IMeshWriter> aWriter)
{
    const std::string format = NormalizeFormat(aWriter->GetFormatName());
    if(m_writers.contains(format))
    {
        throw DuplicateRegisteredWriter{"Writer already registered for format: " + format};
    }
    m_writers.emplace(format, std::move(aWriter));
}

Mesh MeshConverter::Convert(std::istream& aInput, std::string_view aInputFormat, const Transform& aTransform) const
{
    const std::string normalizedInputFormat = NormalizeFormat(aInputFormat);
    const auto readerIt = m_readers.find(normalizedInputFormat);
    if(readerIt == m_readers.end())
    {
        throw NoRegisteredReader{"No reader registered for format: " + std::string{aInputFormat}};
    }

    Mesh mesh = readerIt->second->Read(aInput);

    const glm::mat4& mat = aTransform.Matrix();

    for(auto& v : mesh.vertices)
    {
        v = glm::vec3{mat * glm::vec4{v, 1.0f}};
    }

    if(mesh.normals)
    {
        const glm::mat4 normalMat = glm::transpose(glm::inverse(mat));
        for(auto& n : *mesh.normals)
        {
            n = glm::normalize(glm::vec3{normalMat * glm::vec4{n, 0.0f}});
        }
    }
    return mesh;
}

Mesh MeshConverter::Convert(std::istream& aInput,
                            std::string_view aInputFormat,
                            std::ostream& aOutput,
                            std::string_view aOutputFormat,
                            const Transform& aTransform) const
{
    const std::string normalizedOutputFormat = NormalizeFormat(aOutputFormat);
    const auto writerIt = m_writers.find(normalizedOutputFormat);
    if(writerIt == m_writers.end())
    {
        throw NoRegisteredWriter{"No writer registered for format: " + std::string{aOutputFormat}};
    }

    Mesh mesh = Convert(aInput, aInputFormat, aTransform);

    writerIt->second->Write(aOutput, mesh);
    return mesh;
}
