#include "ArgParser.hpp"

#include <cstring>
#include <iostream>
#include <optional>
#include <string_view>
#include <unordered_set>

static inline bool IsFlag(const char* apArg) { return apArg != nullptr && strncmp(apArg, "--", 2) == 0; }

ArgParser::ArgParser(int argc, const char* argv[]) noexcept
    : m_knownFlags({"--help", "--surface-area", "--volume"})
    , m_knownArgs({"--input", "--output", "--translate", "--scale", "--rotate", "--inside"})
{
    if(argc <= 1)
    {
        return;
    }

    for(int i = 1; i < argc; ++i)
    {
        if(argv[i] == nullptr || !IsFlag(argv[i]))
        {
            continue;
        }

        if((i + 1) < argc && !IsFlag(argv[i + 1]))
        {
            m_args.emplace(argv[i], argv[i + 1]);
            ++i; // Skip the associated value
        }
        else
        {
            m_flags.emplace(argv[i]);
        }
    }
}

[[nodiscard]] std::optional<std::string_view> ArgParser::GetArgValue(std::string_view aArg) const
{
    if(aArg.empty())
    {
        return std::nullopt;
    }
    std::string arg(aArg);
    PrependHyphen(arg);
    const auto it = m_args.find(arg);
    if(it != m_args.end())
    {
        return std::string_view(it->second);
    }

    return std::nullopt;
}

[[nodiscard]] bool ArgParser::HasFlag(std::string_view aArg) const
{
    if(aArg.empty())
    {
        return false;
    }
    std::string arg(aArg);
    PrependHyphen(arg);
    return m_flags.contains(arg);
}

void ArgParser::PrependHyphen(std::string& aArg) const
{
    if(aArg.starts_with("--"))
    {
        return;
    }
    if(aArg.starts_with("-"))
    {
        aArg = "-" + aArg;
        return;
    }
    aArg = "--" + aArg;
}

void ArgParser::DisplayHelpMenu() const noexcept
{
    std::cout << "Usage: shapr --input <path> [options]\n\n"
              << "Required:\n"
              << "  --input <path>                Input mesh file (currently supports .obj)\n\n"
              << "Conversion:\n"
              << "  --output <path>               Output mesh file path (currently supports .stl)\n\n"
              << "Transforms:\n"
              << "  --translate <x,y,z>           Translate mesh by vector\n"
              << "  --scale <s|x,y,z>             Uniform or per-axis scale\n"
              << "  --rotate <angle,x,y,z>        Rotate by angle in degrees around axis\n\n"
              << "Analysis:\n"
              << "  --surface-area                Print mesh surface area\n"
              << "  --volume                      Print mesh volume\n"
              << "  --inside <x,y,z>              Check if point is inside mesh\n"
              << "  --help                        Show this help menu\n";
}

bool ArgParser::HasUnknownFlags() const { return GetFirstUnknownFlag().has_value(); }

std::optional<std::string_view> ArgParser::GetFirstUnknownFlag() const
{
    for(const auto& flag : m_flags)
    {
        if(!m_knownFlags.contains(flag))
        {
            return flag;
        }
    }

    for(const auto& [arg, _] : m_args)
    {
        if(!m_knownArgs.contains(arg))
        {
            return arg;
        }
    }

    return std::nullopt;
}
