#pragma once

#include "IMeshWriter.hpp"

class StlWriter : public IMeshWriter
{
public:
    void Write(std::ostream& aStream, const Mesh& aMesh) const override;
    [[nodiscard]] constexpr std::string_view GetFormatName() const override { return "stl"; }
};
