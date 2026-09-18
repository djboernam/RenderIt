cmake_minimum_required(VERSION 3.24)
project(RenderItNative VERSION 0.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(RENDERIT_BUILD_TESTS "Build tests" ON)
option(RENDERIT_ENABLE_OPENEXR "Enable OpenEXR output support" ON)
option(RENDERIT_ENABLE_SKETCHUP "Enable SketchUp Live C API bridge when the SDK is provided" OFF)
option(RENDERIT_BUILD_STANDALONE "Build standalone renderer executable" ON)

set(SKETCHUP_SDK_ROOT "" CACHE PATH "Trimble SketchUp Desktop SDK root")

add_library(renderit_core SHARED
    src/engine.cpp
    src/cpu_renderer.cpp
    src/image_io.cpp
    src/gpu_stub.cpp
    src/bridge/host_bridge.cpp
)

target_include_directories(renderit_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(renderit_core PRIVATE RENDERIT_BUILD_DLL=1)

find_package(PNG QUIET)
if(PNG_FOUND)
    target_link_libraries(renderit_core PRIVATE PNG::PNG)
    target_compile_definitions(renderit_core PRIVATE RENDERIT_HAS_PNG=1)
endif()

if(RENDERIT_ENABLE_OPENEXR)
    find_package(OpenEXR QUIET)
    if(OpenEXR_FOUND)
        target_link_libraries(renderit_core PRIVATE OpenEXR::OpenEXR)
        target_compile_definitions(renderit_core PRIVATE RENDERIT_HAS_OPENEXR=1)
    endif()
endif()

if(RENDERIT_ENABLE_SKETCHUP)
    target_sources(renderit_core PRIVATE src/bridge/sketchup_live_bridge.cpp)
    target_compile_definitions(renderit_core PRIVATE RENDERIT_SKETCHUP_SDK=1)
    target_include_directories(renderit_core PRIVATE "${SKETCHUP_SDK_ROOT}/headers")
    if(WIN32)
        target_link_libraries(renderit_core PRIVATE "${SKETCHUP_SDK_ROOT}/binaries/sketchup/x64/sketchup.lib")
    endif()
endif()

if(WIN32)
    target_compile_definitions(renderit_core PRIVATE RENDERIT_WINDOWS=1)
endif()

if(RENDERIT_BUILD_STANDALONE)
    add_executable(renderit_cli src/main.cpp)
    target_link_libraries(renderit_cli PRIVATE renderit_core)
endif()

if(RENDERIT_BUILD_TESTS)
    enable_testing()
    if(RENDERIT_BUILD_STANDALONE)
        add_test(NAME renderit_smoke COMMAND renderit_cli)
    endif()
endif()
