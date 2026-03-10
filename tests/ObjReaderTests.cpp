#include "Exception.hpp"
#include "formats/ObjReader.hpp"

#include "Mesh.hpp"

#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <sstream>

TEST_CASE("ObjReader - Load .obj file correctly")
{
    std::ifstream objFile("test_files/test_cube.obj");
    ObjReader reader;
    Mesh mesh = reader.Read(objFile);
    REQUIRE(mesh.vertices.size() == 8);
    REQUIRE(mesh.triangles.size() == 12);
}

TEST_CASE("ObjReader - File has garbage data and throws ObjParseError")
{
    std::istringstream stream("f 99 99 99");
    ObjReader reader;
    REQUIRE_THROWS_AS(reader.Read(stream), ObjParseError);
}
