# AGENTS.md

Repository-specific guidance for coding agents working on `pa-pa-ya`.

## 1) Current project shape (verified)

- Build system: CMake 3.20+ with C++20.
- Target: one executable, `PaPaYa`.
- Dependencies:
  - `extern/sokol` submodule
  - `spdlog` via `FetchContent`
  - `stb` via `FetchContent`
  - `glm` discovered via `find_path`
- Linux build links `X11`, `Xi`, `Xcursor`, `dl`, `pthread`, and `GL`.
- Source discovery is `file(GLOB_RECURSE SOURCES src/*.cpp)`, so new `.cpp` files must live under `src/` unless CMake is updated.
- Runtime assets are copied post-build into the executable directory.

## 2) Current code ownership (verified)

- `src/main.cpp`
  - Bootstraps Sokol only.
  - Creates the `papaya::Game` instance.
  - Forwards `init`, `frame`, `cleanup`, and input callbacks.
- `src/core/Game.*`
  - Owns app lifecycle orchestration, per-frame high-level flow, input handling, and camera orbit state.
- `src/gameplay/Player.*`
  - Owns local player movement state and player model transform generation.
- `src/graphics/Camera.*`
  - Owns projection/view matrix storage and perspective setup.
- `src/graphics/Renderer.*`
  - Owns shader creation, pipeline creation, player mesh creation, and drawing.
- `src/graphics/Mesh.*`
  - Owns procedural mesh generation and GPU buffer creation/destruction.
- `src/world/World.*`
  - Owns current static world mesh state for the grid slice.
- `assets/shaders/unlit_color.*`
  - Current shader pair used by the main render path.

## 3) Current runtime behavior (verified)

- The current slice renders a grid and a capsule player mesh.
- Movement:
  - `WASD` moves relative to camera yaw.
  - `Space` / `Left Shift` move vertically.
- Camera:
  - Mouse movement orbits the camera when the mouse is locked.
  - `Tab` toggles mouse lock.
- `Escape` requests quit.
- The app currently requests OpenGL 3.3 core profile in `src/main.cpp`.

## 4) Development rules

- Keep `src/main.cpp` minimal. It should remain bootstrap/lifecycle wiring, not gameplay or renderer logic.
- Preserve explicit ownership. If you split work into new modules, define clearly what each module owns.
- Prefer incremental structure over speculative engine layers. The project is still a prototype.
- Follow existing naming/style conventions unless there is a strong reason not to:
  - namespace: `papaya`
  - short lowercase directory names (`core`, `graphics`, `assets`, `shaders`)
  - PascalCase C++ type/file names (`Game`, `Camera`, `Mesh`)
  - shared aliases from `src/core/Types.hpp` (`f32`, `u32`, `Vec3`, `Mat4`, etc.)
  - logging/assert helpers from `src/core/Log.hpp`
- Keep claims evidence-based. Do not describe systems, assets, or gameplay that are not present in inspected code.

## 5) Rendering change rules

- Change mesh format, shader inputs, and pipeline layout together.
- Right now `Vertex` is position-only.
- Right now the shader path uses:
  - vertex uniform block slot `0` for `u_mvp`
  - fragment uniform block slot `1` for `u_color`
- Right now there are separate pipelines for triangles and lines.
- Preserve the current OpenGL 3.3 target unless a change deliberately updates platform/rendering requirements.

## 6) Assets and build workflow

- If a runtime feature depends on assets, keep asset loading paths compatible with the post-build asset copy step.
- Standard local workflow:

```sh
cmake -S . -B build
cmake --build build
```

- The executable is expected at `build/bin/PaPaYa`.
- If you add tests, create a real `tests/` directory and wire it through the existing `PA_PA_YA_BUILD_TESTS` option.

## 7) Local reference notes

- `docs/notes/` is local-only reference material and is ignored by git.
- Current note topics are:
  - `docs/notes/engine/`
  - `docs/notes/rendering/`
  - `docs/notes/graphics/`
  - `docs/notes/architecture/`
- Use those files for guidance, not as text to copy into the repository.
- Do not commit, redistribute, or quote large passages from those notes. Several of those files include explicit copyright / no-reproduction notices.
- Summarize ideas in original project-specific language.

## 8) Verification expectations

- After code changes, at minimum run `cmake --build build` if the existing build directory is valid.
- If configure state is stale or missing, run `cmake -S . -B build` first.
- If you change runtime behavior, say exactly how it was verified.
- If runtime behavior was not exercised, say so explicitly.
- Do not modify `docs/notes/` unless the user asks.

## 9) Near-term bias

- Favor work that improves the current playable/renderable slice:
  - movement feel
  - camera behavior
  - scene composition
  - rendering quality
  - asset loading
  - continued separation as gameplay/world/rendering grow
- Avoid broad framework work unless the current prototype clearly needs it.

## 10) Architecture guidance from local reference notes

- Prefer layered ownership and avoid circular dependencies between modules.
- Treat the engine as both runtime code and a tool/content pipeline. If you change runtime asset needs, think about the offline path needed to produce those assets.
- As `Game` is split up, introduce a clear gameplay-foundation layer rather than mixing all game rules directly into renderer/platform code. Likely long-lived concerns include:
  - world/object state
  - player and transformation state
  - camera systems
  - event/message flow between systems
- Keep low-level rendering separate from visibility/world partition decisions. Streaming, culling, and scene selection should decide what gets submitted; the renderer should focus on drawing submitted data.
- Move toward stable asset identifiers/handles once real content arrives. Avoid letting scattered raw file paths become the long-term asset interface.
- Because the target game is persistent multiplayer, avoid assumptions that only work for a purely local single-player architecture. Player state, world state, and transformation state should remain serializable and eventually replicable.
- Do not assume every subsystem should run at the same update rate. Rendering, physics, animation, AI, networking, and streaming may need different servicing policies.
- Do not introduce multithreading by default. First make ownership and data flow explicit. When concurrency becomes necessary, minimize shared mutable state and contention instead of trying to make every API universally lock-heavy.
