#include "MeshConverter.hpp"
#include "Exception.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

void MeshConverter::RegisterReader(std::unique_ptr<IMeshReader> aReader)
{
    m_readers.emplace(aReader->GetFormatName(), std::move(aReader));
}

void MeshConverter::RegisterWriter(std::unique_ptr<IMeshWriter> aWriter)
{
    m_writers.emplace(aWriter->GetFormatName(), std::move(aWriter));
}

Mesh MeshConverter::Convert(std::istream& aInput, std::string_view aInputFormat, const Transform& aTransform) const
{
    const auto readerIt = m_readers.find(std::string{aInputFormat});
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
    const auto writerIt = m_writers.find(std::string{aOutputFormat});
    if(writerIt == m_writers.end())
    {
        throw NoRegisteredWriter{"No writer registered for format: " + std::string{aOutputFormat}};
    }

    Mesh mesh = Convert(aInput, aInputFormat, aTransform);

    writerIt->second->Write(aOutput, mesh);
    return mesh;
}
