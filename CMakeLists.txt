cmake_minimum_required(VERSION 3.16)
project(shapr CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(vendor/Catch2)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE vendor/glm)

add_library(tinyobjloader INTERFACE)
target_include_directories(tinyobjloader INTERFACE vendor/tinyobjloader)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX "src/main\\.cpp$")

if(LIB_SOURCES)
    add_library(shapr_lib STATIC ${LIB_SOURCES})
    target_include_directories(shapr_lib PUBLIC include)
    target_link_libraries(shapr_lib PUBLIC glm tinyobjloader)
else()
    # No .cpp files in src/ yet (aside from main.cpp); use an INTERFACE
    # library so the project still configures while headers-only.
    add_library(shapr_lib INTERFACE)
    target_include_directories(shapr_lib INTERFACE include)
    target_link_libraries(shapr_lib INTERFACE glm tinyobjloader)
endif()

add_executable(shapr src/main.cpp)
target_link_libraries(shapr PRIVATE shapr_lib)

enable_testing()
add_subdirectory(tests)
