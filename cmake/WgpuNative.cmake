set(WGPU_NATIVE_VERSION "v29.0.1.1")

if(APPLE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        set(WGPU_NATIVE_PLATFORM "macos-aarch64")
    else()
        set(WGPU_NATIVE_PLATFORM "macos-x86_64")
    endif()
elseif(WIN32)
    set(WGPU_NATIVE_PLATFORM "windows-x86_64-msvc")
else()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        set(WGPU_NATIVE_PLATFORM "linux-aarch64")
    else()
        set(WGPU_NATIVE_PLATFORM "linux-x86_64")
    endif()
endif()

FetchContent_Declare(
    wgpu_native_release
    URL https://github.com/gfx-rs/wgpu-native/releases/download/${WGPU_NATIVE_VERSION}/wgpu-${WGPU_NATIVE_PLATFORM}-release.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(wgpu_native_release)

add_library(wgpu_native STATIC IMPORTED GLOBAL)
target_include_directories(wgpu_native INTERFACE ${wgpu_native_release_SOURCE_DIR}/include)

if(WIN32)
    set_target_properties(wgpu_native PROPERTIES IMPORTED_LOCATION ${wgpu_native_release_SOURCE_DIR}/lib/wgpu_native.lib)
    target_link_libraries(wgpu_native INTERFACE d3dcompiler ws2_32 userenv bcrypt ntdll opengl32 propsys runtimeobject)
else()
    set_target_properties(wgpu_native PROPERTIES IMPORTED_LOCATION ${wgpu_native_release_SOURCE_DIR}/lib/libwgpu_native.a)
endif()

if(APPLE)
    target_link_libraries(wgpu_native INTERFACE
        "-framework Metal"
        "-framework QuartzCore"
        "-framework Foundation"
        "-framework CoreGraphics"
        "-framework IOKit"
        "-framework IOSurface"
    )
elseif(UNIX)
    target_link_libraries(wgpu_native INTERFACE dl pthread m)
endif()

set(WGPU_NATIVE_SHARED_LIBRARY ${wgpu_native_release_SOURCE_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}wgpu_native${CMAKE_SHARED_LIBRARY_SUFFIX})
if(NOT WIN32 AND EXISTS ${WGPU_NATIVE_SHARED_LIBRARY})
    add_library(wgpu_native_shared SHARED IMPORTED GLOBAL)
    set_target_properties(wgpu_native_shared PROPERTIES
        IMPORTED_LOCATION ${WGPU_NATIVE_SHARED_LIBRARY}
        IMPORTED_NO_SONAME TRUE
    )
    target_include_directories(wgpu_native_shared INTERFACE ${wgpu_native_release_SOURCE_DIR}/include)
endif()
