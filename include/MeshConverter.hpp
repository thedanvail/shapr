#pragma once

#include "IMeshReader.hpp"
#include "IMeshWriter.hpp"
#include "Transform.hpp"

#include <istream>
#include <memory>
#include <ostream>
#include <string_view>
#include <unordered_map>

namespace StringFunctors
{
    /*
     * Instead of using another string allocation to see if a key
     * is in an unordered map, I'm electing to use these functors
     * to allow the STL to accept string_views and anything that can
     * be implicitly converted to such.
     */
    struct TransparentStringHashFunctor
    {
        using is_transparent = void;
        size_t operator()(std::string_view s) const noexcept { return std::hash<std::string_view>{}(s); }
    };

    struct TransparentStringEqualFunctor
    {
        using is_transparent = void;
        bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
    };
} // namespace StringFunctors

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
    std::unordered_map<std::string,
                       std::unique_ptr<IMeshReader>,
                       StringFunctors::TransparentStringHashFunctor,
                       StringFunctors::TransparentStringEqualFunctor>
        m_readers;

    std::unordered_map<std::string,
                       std::unique_ptr<IMeshWriter>,
                       StringFunctors::TransparentStringHashFunctor,
                       StringFunctors::TransparentStringEqualFunctor>
        m_writers;
};
