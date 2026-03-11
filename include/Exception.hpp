#pragma once

#include <stdexcept>
#include <string>

// This is to reduce boilerplate code
#define CODED_EXCEPTION(className)                                                                                     \
    class className : public std::runtime_error                                                                        \
    {                                                                                                                  \
    public:                                                                                                            \
        explicit className(const std::string& aErrorMsg)                                                               \
            : std::runtime_error(aErrorMsg)                                                                            \
        {                                                                                                              \
        }                                                                                                              \
    };

CODED_EXCEPTION(NoRegisteredReader);
CODED_EXCEPTION(NoRegisteredWriter);
CODED_EXCEPTION(DuplicateRegisteredReader);
CODED_EXCEPTION(DuplicateRegisteredWriter);
CODED_EXCEPTION(ObjParseError);
CODED_EXCEPTION(StlWriteError);
CODED_EXCEPTION(CliInputParseError);
CODED_EXCEPTION(ZeroRotationAxisError);

// Don't leak any macros or else
#undef CODED_EXCEPTION
