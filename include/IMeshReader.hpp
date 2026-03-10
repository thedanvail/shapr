#pragma once

#include "Mesh.hpp"

#include <istream>
#include <string_view>

class IMeshReader
{
public:
    virtual ~IMeshReader() = default;

    [[nodiscard]] virtual Mesh Read(std::istream& aStream) const = 0;
    [[nodiscard]] virtual std::string_view GetFormatName() const = 0;
};
