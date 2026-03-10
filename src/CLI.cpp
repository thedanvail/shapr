#include "CLI.hpp"

#include "ArgParser.hpp"
#include "Exception.hpp"
#include "MeshAnalyzer.hpp"
#include "MeshConverter.hpp"
#include "Transform.hpp"
#include "formats/ObjReader.hpp"
#include "formats/StlWriter.hpp"

#include <cerrno>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace
{
    glm::vec3 ParseVec3(std::string_view aStr)
    {
        glm::vec3 v{};
        if(std::sscanf(aStr.data(), "%f,%f,%f", &v.x, &v.y, &v.z) != 3)
        {
            throw Vec3ParseError{"Invalid vec3 format, expected x,y,z: " + std::string{aStr}};
        }
        return v;
    }
} // namespace

int CLI::Run(int argc, const char* argv[])
{
    ArgParser::Init(argc, argv);
    auto& args = ArgParser::GetInstance();

    std::filesystem::path inputPath;
    if(const auto& path = args.GetArgValue("input"))
    {
        inputPath = std::filesystem::path(*path);
    }

    if(inputPath.empty())
    {
        std::cerr << "Error: please provide an input file." << std::endl;
        return 1;
    }

    if(!std::filesystem::exists(inputPath))
    {
        std::cerr << "Error: Input file not found: " << inputPath.string() << std::endl;
        return 1;
    }

    std::ifstream inputFile{inputPath.string()};
    if(!inputFile.good())
    {
        std::cerr << "Error: could not open input file: " << inputPath.string() << std::endl;
        return 1;
    }

    std::string inputFormat = inputPath.extension().string();
    if(inputFormat.empty())
    {
        std::cerr << "Error: could not determine input file type from extension." << std::endl;
        return 1;
    }
    inputFormat = inputFormat.substr(1);

    Transform transform;
    if(const auto val = args.GetArgValue("translate"))
    {
        transform.Translate(ParseVec3(*val));
    }
    if(const auto val = args.GetArgValue("scale"))
    {
        float s{};
        if(val->find(',') != std::string_view::npos)
        {
            transform.Scale(ParseVec3(*val));
        }
        else
        {
            std::sscanf(val->data(), "%f", &s);
            transform.Scale(s);
        }
    }
    if(const auto v = args.GetArgValue("rotate"))
    {
        float angle{};
        glm::vec3 axis{};
        if(std::sscanf(v->data(), "%f,%f,%f,%f", &angle, &axis.x, &axis.y, &axis.z) != 4)
        {
            std::cerr << "Error: --rotate expects format angle,x,y,z" << std::endl;
            return 1;
        }
        transform.Rotate(glm::radians(angle), axis);
    }

    MeshConverter converter;
    Mesh convertedMesh;
    converter.RegisterReader(std::make_unique<ObjReader>());
    converter.RegisterWriter(std::make_unique<StlWriter>());

    const auto outputPath = args.GetArgValue("output");
    if(outputPath)
    {
        std::filesystem::path outputFilePath{std::string{*outputPath}};
        std::ofstream outputFile{outputFilePath.string()};

        if(!outputFile.good())
        {
            std::error_code ec(errno, std::generic_category());
            std::cerr << "Error: could not open output file: " << ec.message() << std::endl;
            return 1;
        }

        std::string outputFormat = outputFilePath.extension().string();
        if(outputFormat.empty())
        {
            std::cerr << "Error: Could not determine output file type from file extension." << std::endl;
            return 1;
        }
        outputFormat = outputFormat.substr(1);

        try
        {
            convertedMesh = converter.Convert(inputFile, inputFormat, outputFile, outputFormat, transform);
        }
        catch(const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    else
    {
        try
        {
            convertedMesh = converter.Convert(inputFile, inputFormat, transform);
        }
        catch(const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }

    if(args.HasFlag("surface-area"))
    {
        std::cout << "Surface Area: " << MeshAnalyzer::SurfaceArea(convertedMesh) << std::endl;
    }

    if(args.HasFlag("volume"))
    {
        std::cout << "Volume: " << MeshAnalyzer::Volume(convertedMesh) << std::endl;
    }

    if(const auto val = args.GetArgValue("inside"))
    {
        std::cout << "Inside: " << (MeshAnalyzer::IsInside(convertedMesh, ParseVec3(*val)) ? "yes" : "no") << std::endl;
    }

    return 0;
}
