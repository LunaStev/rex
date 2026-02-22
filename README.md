# ![Rex Logo](.github/REX_LOGO_WHITE.png)

Rex is a lightweight C++20 game engine/editor codebase with an ECS core, OpenGL renderer, rigid-body physics, and an SDL + RexUI editor.

This repository is intended for both:
- external developers building runtime content/tools on top of Rex
- internal contributors extending engine systems and architecture

## Current Highlights
- ECS-style scene model (`Scene` + component pools)
- OpenGL rendering pipeline with mesh/model support and tonemapping
- Rigid-body physics with:
  - quaternion rotational dynamics
  - Rust core + C++ bridge architecture
  - broadphase candidate generation (bounding-sphere overlap)
  - contact manifold caching/warmstart (2-4 points)
  - TOI-based CCD (swept-sphere approximation)
  - distance joint constraints
- Editor migrated away from Qt to SDL + RexUI (`rex-editor`)
- Next-generation industrial RexUI framework skeleton under `Engine/UI/RexUI/`

## Requirements
- C++20 compiler
- CMake 3.20+
- Rust toolchain (`rustc`, `cargo`)
- SDL2
- OpenGL development libraries
- Freetype

### Fedora-like setup
```bash
sudo dnf install gcc-c++ cmake SDL2-devel freetype-devel mesa-libGL-devel
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## Build
```bash
python3 x.py
```

Manual CMake flow (optional):
```bash
cmake -S . -B build
cmake --build build -j4
```

## Run
```bash
./build/rex-editor
./build/rex-runtime
```

## Quick Runtime Flow
Use `Engine/Runtime/runtime_main.cpp` as the baseline integration example.

Typical frame flow:
1. Create `Window`, `Scene`, `Renderer`, `PhysicsSystem`
2. Create entities/components (`Transform`, `MeshRenderer`, `RigidBodyComponent`, etc.)
3. Per frame:
   - poll events
   - `physics.update(scene, dt)`
   - `renderer.render(scene, camera, view, viewPos, width, height, backbufferFBO)`
   - swap buffers

## Project Layout
```text
Engine/
  Core/        # ECS, math, window/logger
  Graphics/    # renderer, shader, mesh/model
  Physics/     # rigid bodies, world solver, ECS bridge
  UI/          # legacy RexUI + next-gen RexUI skeleton
  EditorRex/   # editor entry point (SDL + RexUI)
  Runtime/     # runtime sample entry point
docs/
  english/     # English developer docs
  korean/      # Korean developer docs
```

## Documentation
Start here:
- `docs/english/overview.md`
- `docs/english/core.md`
- `docs/english/entity.md`
- `docs/english/graphics.md`
- `docs/english/physics.md`
- `docs/english/input.md`
- `docs/english/world.md`
- `docs/english/roadmap.md`

RexUI architecture and execution policy:
- `docs/english/rexui_slate_grade_architecture.md`
- `docs/english/rexui_framework_execution_lock.md`

## Development Notes
- Keep module boundaries explicit (Core -> Framework -> Runtime -> Renderer direction for next-gen UI)
- Avoid direct model mutation from UI in next-gen RexUI path; use command/state flow
- Keep docs synchronized with code changes in physics, rendering, and UI architecture

## Known Scope and Limitations
- The next-gen RexUI framework is currently a design-locked skeleton (interface/TODO phase), not feature-complete yet
- Physics currently focuses on OBB-style rigid body flow and distance joints; broader shape/joint stacks are future work
- Deterministic networking and multithreaded physics solving are not implemented yet

## Contributing
When submitting changes:
1. keep build green (`python3 x.py`)
2. update relevant docs under `docs/english/` (and `docs/korean/` when applicable)
3. document architectural or contract-level changes clearly in PR notes

## License
See `LICENSE`.
