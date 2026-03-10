#pragma once

#include "IMeshReader.hpp"

class ObjReader : public IMeshReader
{
public:
    [[nodiscard]] Mesh Read(std::istream& aStream) const override;
    [[nodiscard]] constexpr std::string_view GetFormatName() const override { return "obj"; }
};
