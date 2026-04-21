# Aiko

Aiko is a C++ game engine/framework project built with CMake.

The repository is organized around a main engine library and a separate examples folder used to test ideas, features, and small standalone applications.

## What is in this repository

### Main project
The main project is split into several modules:

- **AikoCore**: shared utilities, math, events, logging, time, assets, and common types.
- **AikoRenderer**: rendering layer, materials, meshes, textures, cameras, frame buffers, input/display integration, and ImGui support.
- **AikoPhysics**: physics wrapper built around Jolt Physics.
- **Aiko**: the engine layer that connects application flow, scenes, systems, components, assets, and rendering.
- **AikoEditor**: an editor application built on top of the engine.

### Examples project
The `examples/` folder contains small projects that use the engine in different ways.
They work as demos, experiments, and test applications for engine features.

## Top characteristics

- Written in **C++20**.
- Built with **CMake** and separated into clear modules.
- Includes both the **engine** and an **editor application**.
- Has **rendering**, **physics**, **scene/application systems**, and **asset-related code**.
- Contains multiple **example applications** to try different ideas and features.
- Several parts of the project are still **under development** and may change over time.
- Rendering backend is configurable in CMake (`AIKO_BGFX` or `AIKO_NATIVE`).
- `AIKO_NATIVE` is currently **not working / under future development**.

## Examples

- **Sandbox**: a general playground for engine features such as scene objects, lights, textures, and rendering tests.
- **CellularAutomaton**: a cellular automaton simulation with world/chunk/cell logic.
- **NBody**: an N-body style simulation example.
- **LifeParticles**: a particle-based experiment.
- **SortLab**: a small visualization project for sorting algorithms like bubble sort, insertion sort, and selection sort.
- **VoxelWorld**: a voxel world prototype with chunk generation and procedural noise.
- **LoopRecorder**: an audio loop recorder/player/visualizer example.
- **Nes6502**: a NES / 6502 emulator-related project built inside the examples workspace.
- **ShaderToy**: a shader playground example for fullscreen shader experiments.
- **Naiko**: a custom language/compiler experiment with lexer, parser, semantic analysis, emitters, and tests.

## Project structure

```text
.
├── aiko/
│   ├── core/
│   ├── renderer/
│   ├── physics/
│   ├── engine/
│   └── editor/
├── examples/
├── tools/
├── cmake/
└── CMakeLists.txt
```
