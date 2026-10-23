# Wheel of Fear (C++)

C++23 port of the Zig game. Rendering uses wgpu-native with the WGSL shaders in `src/core/render/shaders`; SDL3 handles the window, input and audio.

## Requirements

- CMake 3.24+ and a C++23 compiler
- SDL3, FreeType and Opus (`brew install sdl3 freetype opus` on macOS)
- wgpu-native, stb_image and doctest are downloaded by CMake on first configure

## Build and run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/wheel_of_fear_tests
./build/wheel_of_fear
```

Assets are read from the repository's `assets/` folder. Set `ENGRENAGEM_DEMO` (`showcase`, `freeze`, `repair`, `locker`, `grenade`, ...) to run the scripted demo scenarios.

## Layout

- `src/core`: engine pieces shared by every feature (math, entity, items, input, render, audio, assets, platform)
- `src/mods`: one vertical slice per gameplay feature (world, player, flashlight, mobs, generators, ...)
- `src/game`: the `Game` loop that wires the slices together
