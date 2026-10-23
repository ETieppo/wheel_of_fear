cmake_minimum_required(VERSION 3.24)

project(wheel_of_fear LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

include(FetchContent)
include(cmake/WgpuNative.cmake)

find_package(SDL3 REQUIRED CONFIG)
find_package(Freetype REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPUS REQUIRED IMPORTED_TARGET opus)

FetchContent_Declare(
    stb
    URL https://github.com/nothings/stb/archive/003501f6d7a8668ce8b6d875e34dd97ad0d5d5b5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(
    doctest
    URL https://github.com/doctest/doctest/archive/refs/tags/v2.4.11.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_SUBDIR header_only
)
FetchContent_MakeAvailable(stb doctest)

add_library(doctest_header INTERFACE)
target_include_directories(doctest_header SYSTEM INTERFACE ${doctest_SOURCE_DIR})

add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE ${stb_SOURCE_DIR})

set(WHEEL_OF_FEAR_ASSET_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(WHEEL_OF_FEAR_SHADER_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/core/render/shaders")

file(GLOB_RECURSE WHEEL_OF_FEAR_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mods/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.cpp
)

add_library(wheel_of_fear_engine STATIC ${WHEEL_OF_FEAR_SOURCES})
target_include_directories(wheel_of_fear_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(wheel_of_fear_engine PUBLIC
    WHEEL_OF_FEAR_ASSET_DIRECTORY="${WHEEL_OF_FEAR_ASSET_DIRECTORY}"
    WHEEL_OF_FEAR_SHADER_DIRECTORY="${WHEEL_OF_FEAR_SHADER_DIRECTORY}"
)
target_link_libraries(wheel_of_fear_engine PUBLIC
    SDL3::SDL3
    Freetype::Freetype
    PkgConfig::OPUS
    wgpu_native
    stb_image
)
if(MSVC)
    target_compile_options(wheel_of_fear_engine PRIVATE /W4 /permissive-)
else()
    target_compile_options(wheel_of_fear_engine PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wno-missing-field-initializers)
endif()

add_executable(wheel_of_fear ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(wheel_of_fear PRIVATE wheel_of_fear_engine)

file(GLOB_RECURSE WHEEL_OF_FEAR_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
add_executable(wheel_of_fear_tests ${WHEEL_OF_FEAR_TEST_SOURCES})
target_link_libraries(wheel_of_fear_tests PRIVATE wheel_of_fear_engine doctest_header)

enable_testing()
add_test(NAME wheel_of_fear_tests COMMAND wheel_of_fear_tests)
