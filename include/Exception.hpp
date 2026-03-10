#pragma once

#include <stdexcept>

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

class Vec3ParseError : public std::runtime_error
{
public:
    explicit Vec3ParseError(const std::string& aErrorMsg)
        : std::runtime_error(aErrorMsg)
    {
    }
};
