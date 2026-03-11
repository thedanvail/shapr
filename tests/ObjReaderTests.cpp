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

TEST_CASE("ObjReader - Out of bounds normal index throws ObjParseError")
{
    std::istringstream stream("v 0 0 0\n"
                              "v 1 0 0\n"
                              "v 0 1 0\n"
                              "vn 0 0 1\n"
                              "f 1//2 2//2 3//2\n");
    ObjReader reader;
    REQUIRE_THROWS_AS(reader.Read(stream), ObjParseError);
}

TEST_CASE("ObjReader - Out of bounds texcoord index throws ObjParseError")
{
    std::istringstream stream("v 0 0 0\n"
                              "v 1 0 0\n"
                              "v 0 1 0\n"
                              "vt 0 0\n"
                              "f 1/2 2/2 3/2\n");
    ObjReader reader;
    REQUIRE_THROWS_AS(reader.Read(stream), ObjParseError);
}

TEST_CASE("ObjReader - Mixed normal presence resets normals to nullopt")
{
    std::istringstream stream("v 0 0 0\n"
                              "v 1 0 0\n"
                              "v 0 1 0\n"
                              "v 0 0 1\n"
                              "vn 0 0 1\n"
                              "f 1//1 2//1 3//1\n"
                              "f 1//1 3//1 4\n");
    ObjReader reader;
    Mesh mesh = reader.Read(stream);

    REQUIRE_FALSE(mesh.normals.has_value());
}

TEST_CASE("ObjReader - Face with fewer than 3 vertices throws ObjParseError")
{
    std::istringstream stream("v 0 0 0\n"
                              "v 1 0 0\n"
                              "f 1 2\n");
    ObjReader reader;
    REQUIRE_THROWS_AS(reader.Read(stream), ObjParseError);
}
