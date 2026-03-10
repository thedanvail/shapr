#pragma once

#include "Mesh.hpp"

#include <ostream>
#include <string_view>

class IMeshWriter
{
public:
    virtual ~IMeshWriter() = default;

    virtual void Write(std::ostream& aStream, const Mesh& aMesh) const = 0;
    [[nodiscard]] virtual std::string_view GetFormatName() const = 0;
};
