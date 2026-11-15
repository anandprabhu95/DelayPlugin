set(DEFAULT_BUILD "Reverb")
include(cmake/CPM.cmake/cmake/CPM.cmake)

# ===========================================================
set(PLUGIN "${PLUGIN}" CACHE STRING "Plugin to build.")
if(DEFINED PLUGIN AND PLUGIN STREQUAL "")
	message("No plugin name provided. Setting var PLUGIN to default value.")
	set(PLUGIN ${DEFAULT_BUILD})
endif()
message("Building ${PLUGIN} plugin")

option(ACTIONS "Github Actions" FALSE)
message("Actions: ${ACTIONS}")

set(IS_LOCAL_JUCE_SOURCE TRUE)  # set TRUE if building from local copy of JUCE framework source.
                                 # if IS_LOCAL_JUCE_SOURCE is set to FALSE, cmake will download 
								 # the JUCE_VERSION specified below.
set(JUCE_VERSION 8.0.8)

# ===========================================================
cmake_minimum_required(VERSION 3.5)

project(AudioPlugins)

set(CMAKE_CXX_STANDARD 17)

message("Compiler: ${CMAKE_CXX_COMPILER_ID}")

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)


if(NOT ACTIONS AND IS_LOCAL_JUCE_SOURCE)
	CPMAddPackage(
		NAME mylocal
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/local/juce-8.0.8-windows/juce # Path to local Juce Framework source 
		)
else()
	CPMAddPackage(
		NAME JUCE
		GITHUB_REPOSITORY juce-framework/JUCE
		GIT_TAG ${JUCE_VERSION}
		VERSION ${JUCE_VERSION}
		SOURCE_DIR ${LIB_DIR}/juce
		)
endif()

#set(CPM_USE_LOCAL_PACKAGES TRUE CACHE BOOL "Use already present CPM sources")

if(MSVC)
    add_compile_options(/Wall)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(plugins)
