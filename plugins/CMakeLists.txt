cmake_minimum_required(VERSION 3.5)

project(AudioPlugin VERSION 1.4.0)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/Delay")

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME AudioPlugin
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE Aupl
    PLUGIN_CODE Aupl
    FORMATS VST3 Standalone
    PRODUCT_NAME "Frecurse"
)

# Sets the source files of the plugin project.
set(SOURCE_FILES 
    source/PluginEditor.cpp 
    source/PluginProcessor.cpp
)

# Optional; includes header files in the project file tree in Visual Studio
set(HEADER_FILES 
    ${INCLUDE_DIR}/Headers.h
    ${INCLUDE_DIR}/PluginEditor.h 
    ${INCLUDE_DIR}/PluginProcessor.h)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

# Sets the include directories of the plugin project.
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#juce_generate_juce_header(${PROJECT_NAME})

# Links to all necessary dependencies. The present ones are recommended by JUCE.
# If you use one of the additional modules, like the DSP module, you need to specify it here.
target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
        juce::juce_audio_utils
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
)

target_link_libraries(
  ${PROJECT_NAME} PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
                         juce::juce_recommended_warning_flags
)

# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)

# Enables strict C++ warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
set_source_files_properties(${SOURCE_FILES} PROPERTIES COMPILE_OPTIONS "${PROJECT_WARNINGS_CXX}")

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            _SILENCE_CXX17_ALIGNED_STORAGE_DEPRICATION_WARNING
    )
endif()