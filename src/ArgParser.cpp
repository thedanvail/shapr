#include "ArgParser.hpp"

#include <cstring>
#include <optional>
#include <string_view>

static inline bool IsFlag(const char* apArg) { return apArg != nullptr && strncmp(apArg, "--", 2) == 0; }

ArgParser::ArgParser(int argc, const char* argv[]) noexcept
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
