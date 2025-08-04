cmake_minimum_required(VERSION 3.5)

project(AudioPlugins)

set(CMAKE_CXX_STANDARD 23)

message("Compiler: ${CMAKE_CXX_COMPILER_ID}")

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
include(cmake/CPM.cmake)

CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 8.0.8
    VERSION 8.0.8
    SOURCE_DIR ${LIB_DIR}/juce
)


if(MSVC)
    add_compile_options(/WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(plugins)
