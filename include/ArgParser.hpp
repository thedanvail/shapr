#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

class ArgParser
{
public:
    explicit ArgParser(int argc, const char* argv[]) noexcept;
    ~ArgParser() = default;
    ArgParser(const ArgParser&) = default;
    ArgParser(ArgParser&&) = default;
    ArgParser& operator=(const ArgParser&) = delete;
    ArgParser& operator=(ArgParser&&) = delete;

    [[nodiscard]] std::optional<std::string_view> GetArgValue(std::string_view aArg) const;
    [[nodiscard]] bool HasFlag(std::string_view aArg) const;
    [[nodiscard]] bool HasUnknownFlags() const;
    [[nodiscard]] std::optional<std::string_view> GetFirstUnknownFlag() const;
    void DisplayHelpMenu() const noexcept;

private:
    void PrependHyphen(std::string& aArg) const;

    std::unordered_map<std::string, std::string> m_args;
    std::unordered_set<std::string> m_flags;
    const std::unordered_set<std::string_view> m_knownFlags;
    const std::unordered_set<std::string_view> m_knownArgs;
};
