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
include(cmake/EmbeddedAssets.cmake)

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

option(WHEEL_OF_FEAR_HOT_RELOAD "Build the game as a module that reloads while the window stays open" OFF)

function(wheel_of_fear_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wno-missing-field-initializers)
    endif()
endfunction()

if(WHEEL_OF_FEAR_HOT_RELOAD AND TARGET wgpu_native_shared)
    set(WHEEL_OF_FEAR_WGPU_LIBRARY wgpu_native_shared)
else()
    set(WHEEL_OF_FEAR_WGPU_LIBRARY wgpu_native)
endif()

file(GLOB_RECURSE WHEEL_OF_FEAR_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mods/*.cpp
)
list(APPEND WHEEL_OF_FEAR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameApplication.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameSimulation.cpp
)

add_library(wheel_of_fear_engine OBJECT ${WHEEL_OF_FEAR_SOURCES})
target_include_directories(wheel_of_fear_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
wheel_of_fear_add_embedded_assets(wheel_of_fear_engine
    "${WHEEL_OF_FEAR_ASSET_DIRECTORY}"
    "${WHEEL_OF_FEAR_SHADER_DIRECTORY}"
)
target_link_libraries(wheel_of_fear_engine PUBLIC
    SDL3::SDL3
    Freetype::Freetype
    PkgConfig::OPUS
    ${WHEEL_OF_FEAR_WGPU_LIBRARY}
    stb_image
)
wheel_of_fear_enable_warnings(wheel_of_fear_engine)
if(NOT MSVC)
    target_compile_options(wheel_of_fear_engine PUBLIC -ffp-contract=off)
endif()

if(WHEEL_OF_FEAR_HOT_RELOAD)
    set(WHEEL_OF_FEAR_GAME_MODULE_STAMP_PATH "${CMAKE_CURRENT_BINARY_DIR}/wheel_of_fear_game.stamp")

    set_target_properties(wheel_of_fear_engine PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
    target_compile_options(wheel_of_fear_engine PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-gnu-unique>)

    add_library(wheel_of_fear_game MODULE ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameModule.cpp)
    set_target_properties(wheel_of_fear_game PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
    target_compile_options(wheel_of_fear_game PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-gnu-unique>)
    target_link_libraries(wheel_of_fear_game PRIVATE wheel_of_fear_engine)
    wheel_of_fear_enable_warnings(wheel_of_fear_game)
    add_custom_command(TARGET wheel_of_fear_game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E touch ${WHEEL_OF_FEAR_GAME_MODULE_STAMP_PATH}
    )

    add_executable(wheel_of_fear
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/game/HotReloadGameRunner.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/platform/Window.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/platform/SharedLibrary.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/platform/SharedLibraryWatcher.cpp
    )
    target_include_directories(wheel_of_fear PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_compile_definitions(wheel_of_fear PRIVATE
        WHEEL_OF_FEAR_GAME_MODULE_PATH="$<TARGET_FILE:wheel_of_fear_game>"
        WHEEL_OF_FEAR_GAME_MODULE_STAMP_PATH="${WHEEL_OF_FEAR_GAME_MODULE_STAMP_PATH}"
    )
    target_link_libraries(wheel_of_fear PRIVATE SDL3::SDL3)
    add_dependencies(wheel_of_fear wheel_of_fear_game)
else()
    add_executable(wheel_of_fear
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/game/StaticGameRunner.cpp
    )
    target_link_libraries(wheel_of_fear PRIVATE wheel_of_fear_engine)
endif()
wheel_of_fear_enable_warnings(wheel_of_fear)

file(GLOB_RECURSE WHEEL_OF_FEAR_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
add_executable(wheel_of_fear_tests ${WHEEL_OF_FEAR_TEST_SOURCES})
target_link_libraries(wheel_of_fear_tests PRIVATE wheel_of_fear_engine doctest_header)
target_include_directories(wheel_of_fear_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
target_compile_definitions(wheel_of_fear_tests PRIVATE
    WHEEL_OF_FEAR_ASSET_DIRECTORY="${WHEEL_OF_FEAR_ASSET_DIRECTORY}"
    WHEEL_OF_FEAR_SHADER_DIRECTORY="${WHEEL_OF_FEAR_SHADER_DIRECTORY}"
)

enable_testing()
add_test(NAME wheel_of_fear_tests COMMAND wheel_of_fear_tests)
