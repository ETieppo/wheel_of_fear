# Wheel of Fear (C++)

## Requirements

- CMake 3.24+ and a C++23 compiler
- SDL3, FreeType and Opus (`brew install sdl3 freetype opus` on macOS)
- wgpu-native, stb_image and doctest are downloaded by CMake on first configure

## Build and run

```sh
make run
```

Assets are read from the repository's `assets/` folder. Set `ENGRENAGEM_DEMO` (`showcase`, `freeze`, `repair`, `locker`, `grenade`, ...) to run the scripted demo scenarios.

## Hot reload

ensure inotify-tools first in your path:

```bash
sudo pacman -S inotify-tools  # if you use arch
sudo chmod +x watch.sh        # give file permision
```

then:

```bash
make dev
```

#### or

Configure with `-DWHEEL_OF_FEAR_HOT_RELOAD=ON` to build everything in `src/core`, `src/mods` and `src/game/Game.cpp` as a module that a small launcher reloads while the window stays open

```sh
cmake -S . -B build-hot -DCMAKE_BUILD_TYPE=Release -DWHEEL_OF_FEAR_HOT_RELOAD=ON
cmake --build build-hot -j
./build-hot/wheel_of_fear
```

While the game runs, edit any file and rebuild only the module from another terminal; the game swaps it in on the next frame:

```sh
cmake --build build-hot --target wheel_of_fear_game
```

The launcher keeps only the SDL window. On each reload the GPU device, audio, art and gameplay are rebuilt on that window, and the game resumes the current phase on its starting map; live mobs, items and player progress are not kept. A module that fails to build its game is rejected and the previous one keeps running. The launcher's own code still needs a restart: `src/main.cpp`, `src/game/HotReloadGameRunner.*`, `src/game/GameModuleApiProps.hpp`, `src/core/platform/SharedLibrary*`, and `src/core/platform/Window.*` (the window object is created by the launcher, so changing its members breaks the module). Changes to `CMakeLists.txt` need a reconfigure.

## Layout

- `src/core`: engine pieces shared by every feature (math, entity, items, input, render, audio, assets, platform)
- `src/mods`: one vertical slice per gameplay feature (world, player, flashlight, mobs, generators, ...)
- `src/game`: the `Game` loop that wires the slices together
