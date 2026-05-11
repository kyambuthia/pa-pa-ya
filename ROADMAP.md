# ROADMAP.md

## 1) Project direction

`PaPaYa` is not intended to become a general-purpose engine.

It is a game-specific engine and game built together for one product: a persistent online science-fiction world where players control a transformable machine body. That machine must support three core traversal modes:

- walking on foot as a humanoid machine
- reconfiguring into a fast ground vehicle
- transitioning into a flight-capable spacecraft

The world target is larger than a single level. The game needs multiple planetary bodies, travel between surface and space, and a persistent shared simulation that can support many players over time.

## 2) Current starting point (verified)

The current repository is still at an early prototype stage:

- one C++20 executable built with CMake
- Sokol-based application bootstrap in `src/main.cpp`
- `Game` currently orchestrates lifecycle, input, and camera orbit while delegating gameplay/world/render work to dedicated modules
- `Player` owns local player movement state
- `World` owns the current grid-slice world mesh
- `Renderer` owns shader setup, pipelines, player mesh setup, and drawing
- current render slice: a flat grid plus a capsule player mesh
- current controls: `WASD`, vertical movement, orbit camera, mouse lock toggle
- current graphics path: OpenGL 3.3 core profile, position-only vertices, one unlit shader path, and separate triangle/line pipelines

This means the roadmap below is mostly forward-looking product and technical planning, not a description of features that already exist.

## 3) Product principles

1. Build for one game, not for every possible game.
2. Prefer vertical slices that prove the final play loop early.
3. Keep ownership explicit: app bootstrap, simulation, world, rendering, assets, networking, and tools should not collapse into one class forever.
4. Solve scale in layers: on-foot scale, vehicle scale, planetary scale, then persistent multiplayer scale.
5. Add complexity only when the current slice proves the need.

## 4) Technical requirements

### 4.1 Core application architecture

The current `Game` class is sufficient for a prototype but not for the target scope.

To support the planned game, the codebase will need clearly owned modules for at least:

- app lifecycle and platform integration
- input and control mapping
- player state and transformation state machine
- world simulation
- rendering
- assets/content loading
- networking
- persistence/backend integration
- debug and profiling tools

### 4.2 Multi-scale spatial simulation

Because the target includes walking-scale control, ground vehicles, spacecraft, and multiple planets, the engine must handle very different spatial scales without precision breakdown.

Minimum requirements:

- stable transforms at local character scale and large world scale
- a precision strategy for planetary and space distances
- support for local spaces, planetary spaces, and travel spaces
- origin management or an equivalent large-world coordinate strategy
- simulation rules for gravity, orientation, and reference frames per environment

### 4.3 World and planet systems

The target world needs more than a single handcrafted test scene.

Requirements:

- data-driven definitions for planets and other world regions
- streaming of terrain, props, and gameplay entities by distance and relevance
- support for surface, atmosphere, orbit, and deep-space contexts
- runtime loading/unloading of world data without blocking the frame loop
- planet-specific parameters such as size, gravity, sky/atmosphere rules, and landing/travel metadata

### 4.4 Player transformation and traversal

The core player fantasy depends on transformation across movement modes, so this is a first-class engine requirement.

Requirements:

- one player identity that persists across all traversal modes
- a transformation state machine with explicit allowed transitions
- different movement controllers for walker, ground vehicle, and spacecraft modes
- camera behavior that adapts per mode without rewriting the whole camera system each time
- collision, acceleration, braking, turning, takeoff, flight, and landing behavior per mode
- animation and/or part-reconfiguration support for visible transformations

### 4.5 Rendering requirements

The current renderer is enough for a prototype slice, but it is not enough for the full target.

The game will require:

- asset-loaded meshes instead of only procedural placeholder meshes
- richer vertex formats than position-only data
- textured and lit materials
- terrain and planetary rendering with level-of-detail support
- distant-body rendering for planets, moons, and large landmarks
- sky, atmosphere, and space rendering paths
- instancing or equivalent batching for repeated objects
- debug rendering for streaming, physics, and network visibility

Open technical concern:

- The current OpenGL 3.3 path is a verified baseline. Whether that ceiling remains sufficient for the long-term target must be validated with real slices, not guessed in advance.

### 4.6 Physics and movement

The target game needs more than free-flying transform updates.

Requirements:

- grounded movement with collision and slope handling
- wheeled or vehicle-style motion for high-speed ground travel
- flight dynamics suitable for atmospheric and/or space travel
- transition handling between ground, air, and space contexts
- broadphase/narrowphase collision support that can scale beyond one player actor
- deterministic-enough simulation for networking and correction workflows

### 4.7 Networking and persistence

Because the target is a persistent online multiplayer game, networking is not optional infrastructure.

Requirements:

- dedicated server architecture or an equivalent authoritative simulation model
- replication of player state, movement state, and transformation state
- area-of-interest or relevance filtering so each client only receives needed state
- persistent storage for player identity, location, active form, and world state required by the game
- reconnect-safe session flow
- server/client correction and prediction strategy for responsive controls
- service boundaries that can expand as concurrency increases

### 4.8 Asset and content pipeline

The current project already copies runtime assets after build. The target game will need a much stronger content pipeline.

Requirements:

- import pipeline for meshes, materials, animations, and world data
- stable runtime asset identifiers and load paths
- asynchronous asset loading
- placeholder/fallback assets for missing content
- per-planet and per-region content configuration
- validation tools for broken references and bad data

### 4.9 Tooling and observability

This game cannot be built efficiently without internal tools.

Requirements:

- in-game debug UI for simulation, transform mode, streaming state, and performance
- profiling hooks for frame time, streaming, memory, and network traffic
- logging and diagnostics for asset failures, world streaming, and replication issues
- developer commands for teleport, travel, spawning, and mode switching
- offline inspection tools or data dumps for world definitions

### 4.10 Testing and verification

The current project has a test option in CMake but no verified `tests/` directory yet.

The roadmap should include:

- unit tests for math, transforms, and state machines
- simulation tests for movement mode transitions
- build verification in CI or equivalent automation
- streaming stress tests
- multiplayer soak tests
- regression checks for serialization, save/load, and replication

## 5) Phased roadmap

### Phase 0 — Prototype hardening

Goal: turn the current render/movement prototype into a maintainable base.

Deliverables:

- split crowded responsibilities out of `src/core/Game.*`
- introduce explicit modules for player, rendering, world, and input
- keep `src/main.cpp` as bootstrap only
- add basic asset loading for real meshes/shaders beyond hardcoded placeholders
- establish a minimal test target and build verification path

### Phase 1 — Ground traversal vertical slice

Goal: prove the core feel of controlling a transformable machine on a planetary surface.

Deliverables:

- replace capsule placeholder with a transformable player placeholder asset
- add grounded movement, collision, and camera polish
- implement walker mode and ground vehicle mode
- implement visible transformation between those two modes
- add a small streamed terrain or terrain-like test area

### Phase 2 — Flight vertical slice

Goal: prove the transition from surface traversal to flight.

Deliverables:

- spacecraft mode
- takeoff and landing flow
- atmospheric flight controls
- sky/atmosphere rendering improvements
- transition rules between surface and high-altitude travel states

### Phase 3 — Planetary scale slice

Goal: prove that the game can support more than one local play area.

Deliverables:

- planet definitions driven by data
- world streaming by region/chunk/cell
- at least two distinct planetary environments
- orbital or large-scale travel presentation between regions
- precision strategy validated under large travel distances

### Phase 4 — Shared online slice

Goal: prove that the traversal loop works in a persistent multiplayer setting.

Deliverables:

- authoritative server prototype
- replicated player movement and transformation state
- persistence for login/session and player position/state
- interest management for nearby players and relevant world entities
- multiplayer debugging tools

### Phase 5 — Persistent world foundation

Goal: move from a multiplayer prototype to an operable persistent world.

Deliverables:

- persistent world services and storage boundaries
- recovery flows for disconnects and reconnects
- world/event/state persistence rules
- operational metrics, logging, and failure visibility
- content workflows for expanding planets, regions, and traversal spaces

## 6) Immediate engineering priority

The next serious development target should not be full-scale online infrastructure yet.

The highest-value next slice is:

1. add an explicit player mode/state machine
2. implement walker-to-vehicle transformation
3. add grounded terrain and collision
4. replace the capsule placeholder with an asset-driven player placeholder
5. validate the feel of mode switching before expanding to flight and networking

That sequence fits the current codebase, proves the core fantasy early, and avoids building large systems before the moment-to-moment game loop exists.

## 7) Open decisions that should be resolved early

These are important but not yet specified by the current repo or the request:

- target player concurrency per world/server
- whether surface-to-space travel must be fully seamless or can use managed transitions
- how much of the world is procedural versus authored
- whether spacecraft handling is arcade-like, sim-like, or hybrid
- which backend stack will own persistence and live operations

Those decisions will heavily affect networking, world partitioning, physics, and rendering priorities.
