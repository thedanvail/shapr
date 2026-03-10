#pragma once

#include "IMeshReader.hpp"
#include "IMeshWriter.hpp"
#include "Transform.hpp"

#include <istream>
#include <memory>
#include <ostream>
#include <string_view>
#include <unordered_map>

class MeshConverter
{
public:
    void RegisterReader(std::unique_ptr<IMeshReader> aReader);
    void RegisterWriter(std::unique_ptr<IMeshWriter> aWriter);

    /*
     * Electing to use two overloaded methods in case any analysis
     * wants to be done before writing the mesh to the output format.
     */
    [[nodiscard]] Mesh
    Convert(std::istream& aInput, std::string_view aInputFormat, const Transform& aTransform = Transform{}) const;

    [[nodiscard]] Mesh Convert(std::istream& aInput,
                               std::string_view aInputFormat,
                               std::ostream& aOutput,
                               std::string_view aOutputFormat,
                               const Transform& aTransform = Transform{}) const;

private:
    std::unordered_map<std::string, std::unique_ptr<IMeshReader>> m_readers;
    std::unordered_map<std::string, std::unique_ptr<IMeshWriter>> m_writers;
};
