
# ===========================================================
set(CURR_BUILD "Reverb")

message("Building ${CURR_BUILD} plugin")
# ===========================================================

cmake_minimum_required(VERSION 3.5)

project(AudioPlugins)

set(CMAKE_CXX_STANDARD 17)

message("Compiler: ${CMAKE_CXX_COMPILER_ID}")

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
include(cmake/CPM.cmake)

execute_process(
    COMMAND ping www.google.com -n 2 -w 1000
    RESULT_VARIABLE NO_CONNECTION
)
if(NO_CONNECTION GREATER 0)
	CPMAddPackage(
		NAME mylocal
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/local/juce-8.0.8-windows/juce
		)
else()
	CPMAddPackage(
		NAME JUCE
		GITHUB_REPOSITORY juce-framework/JUCE
		GIT_TAG 8.0.8
		VERSION 8.0.8
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
