# PaPaYa

`PaPaYa` is a C++20 Sokol-based prototype for a game-specific engine and game.

The long-term target is a persistent online science-fiction world built around a transformable player machine that can operate in multiple traversal modes. The current repository is still in prototype form.

## Current prototype

- flat grid world slice
- capsule player mesh
- third-person orbit camera
- `WASD` movement relative to camera yaw
- `Space` / `Left Shift` vertical movement
- `Tab` toggles mouse lock
- `Escape` requests quit

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/bin/PaPaYa
```

Assets are copied into `build/bin/assets` as part of the build.

## Project layout

- `src/main.cpp` — Sokol bootstrap and app callbacks
- `src/core` — app-level orchestration
- `src/gameplay` — player gameplay state
- `src/world` — current world state
- `src/graphics` — camera, mesh generation, and rendering

## Roadmap

See `ROADMAP.md` for the planned technical direction.
