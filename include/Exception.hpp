#pragma once

#include <stdexcept>
#include <string>

class NoRegisteredReader : public std::runtime_error
{
public:
    explicit NoRegisteredReader(const std::string& aErrorMsg)
        : std::runtime_error(aErrorMsg)
    {
    }
};

class NoRegisteredWriter : public std::runtime_error
{
public:
    explicit NoRegisteredWriter(const std::string& aErrorMsg)
        : std::runtime_error(aErrorMsg)
    {
    }
};

class DuplicateRegisteredReader : public std::runtime_error
{
public:
    explicit DuplicateRegisteredReader(const std::string& aErrorMsg)
        : std::runtime_error(aErrorMsg)
    {
    }
};

class DuplicateRegisteredWriter : public std::runtime_error
{
public:
    explicit DuplicateRegisteredWriter(const std::string& aErrorMsg)
        : std::runtime_error(aErrorMsg)
    {
    }
};

class ObjParseError : public std::runtime_error
{
public:
    explicit ObjParseError(const std::string& aErrorMsg)
        : std::runtime_error(aErrorMsg)
    {
    }
};

class StlWriteError : public std::runtime_error
{
public:
    explicit StlWriteError(const std::string& aErrorMsg)
        : std::runtime_error(aErrorMsg)
    {
    }
};

class CliInputParseError : public std::runtime_error
{
public:
    explicit CliInputParseError(const std::string& aErrorMsg)
        : std::runtime_error(aErrorMsg)
    {
    }
};

class ZeroRotationAxisError : public std::runtime_error
{
public:
    explicit ZeroRotationAxisError(const std::string& aErrorMsg)
        : std::runtime_error(aErrorMsg)
    {
    }
};
