#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

class ArgParser
{
public:
    static void Init(const int argc, const char* argv[]) noexcept;
    static void Reset() noexcept;
    static ArgParser& GetInstance();

    ~ArgParser() = default;
    ArgParser(const ArgParser&) = delete;
    ArgParser(ArgParser&&) = delete;
    ArgParser& operator=(const ArgParser&) = delete;
    ArgParser& operator=(ArgParser&&) = delete;

    [[nodiscard]] std::optional<std::string_view> GetArgValue(std::string_view aArg) const;
    [[nodiscard]] bool HasFlag(std::string_view aArg) const;

private:
    ArgParser() = default;
    void PrependHyphen(std::string& aArg) const;

    bool m_hasBeenInitialized = false;
    std::unordered_map<std::string, std::string> m_args;
    std::unordered_set<std::string> m_flags;
};
