# ![Rex Logo](.github/REX_LOGO_WHITE.png)

Rex is a C++20 game engine/editor codebase with:
- ECS scene/runtime core
- OpenGL real-time renderer (Deferred pipeline)
- Rust-based rigid-body physics core with C++ integration
- SDL + RexUI editor/runtime tooling

This repository is maintained as internal+external developer documentation and source.

## What Is Implemented
- Deferred rendering pipeline with modular passes:
  - Shadow pass (cascaded directional shadow atlas)
  - GBuffer pass
  - Lighting pass (Cook-Torrance PBR)
  - Post process pass (SSAO, Bloom, ACES tone mapping, HDR)
  - UI pass reservation
- Multi-light scene support:
  - Directional / Point / Spot / Area light types
  - Light manager + view-space light culling (CPU-side)
- Graphics module split by responsibility:
  - `Engine/Graphics/Core`
  - `Engine/Graphics/Lighting`
  - `Engine/Graphics/Material`
  - `Engine/Graphics/Pipeline`
  - `Engine/Graphics/PostProcess`
  - `Engine/Graphics/HDR`
  - `Engine/Graphics/Culling`
- Rust physics integration through `PhysicsSystem` and FFI bridge
- Qt-free editor path (`rex-editor`) and runtime sandbox (`rex-runtime`)

## Requirements
- C++20 compiler
- CMake 3.20+
- Python 3
- Rust toolchain (`rustc`, `cargo`)
- SDL2 development libraries
- OpenGL development libraries
- Freetype

Fedora-like setup:
```bash
sudo dnf install gcc-c++ cmake python3 SDL2-devel freetype-devel mesa-libGL-devel
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## Build
Use the project build utility:
```bash
python3 x.py
```

Other useful commands:
```bash
python3 x.py configure
python3 x.py build
python3 x.py run editor
python3 x.py run runtime
python3 x.py clean
```

## Run
```bash
./build/rex-editor
./build/rex-runtime
```

## Runtime Sandbox (Visual Fidelity Test)
`Engine/Runtime/runtime_main.cpp` is configured as a stress/demo scene for the upgraded graphics stack.

It includes:
- directional shadowed key light
- animated point + spot lights
- area light ring
- 128 stress point lights for many-light tests
- HDR + Bloom + SSAO + tone mapping path through the Deferred pipeline

Runtime controls:
- `WASD + QE`: move camera
- `Shift`: speed boost
- `RMB hold + mouse`: freelook
- `LMB`: raycast and apply impulse
- `Space`: spawn projectile cube
- `B`: spawn cube
- `J`: create distance joint between latest dynamic bodies
- `R`: impulse burst
- `T`: torque burst
- `G`: toggle gravity
- `P`: pause physics
- `F1`: bloom toggle
- `F2`: auto exposure toggle
- `F3/F4`: exposure down/up
- `F5/F6`: bloom strength down/up
- `L`: stress light on/off
- `K`: stress light animation on/off
- `Delete`: remove last dynamic entity
- `Esc`: quit

## Project Layout
```text
Engine/
  Core/        # ECS, math, logger, window
  Graphics/    # modular rendering framework + renderer facade
  Physics/     # C++ bridge layer over Rust physics core
  Rust/        # Rust physics crates
  UI/          # RexUI legacy + next-gen framework skeleton
  EditorRex/   # editor entry
  Runtime/     # runtime sandbox entry
docs/
  english/     # English developer docs
  korean/      # Korean developer docs
```

## Documentation
Start with:
- `docs/english/overview.md`
- `docs/english/core.md`
- `docs/english/entity.md`
- `docs/english/graphics.md`
- `docs/english/physics.md`
- `docs/english/input.md`
- `docs/english/world.md`
- `docs/english/roadmap.md`

Korean set mirrors this under `docs/korean/`.

## Current Scope Notes
- Forward+ tile/cluster light culling is not finished yet (current many-light culling is CPU-side ranking)
- IBL environment-map workflow is partially scaffolded but not fully production-complete
- Next-gen RexUI is architecture-first skeleton stage (not fully feature-complete)

## Contributing
When you change engine behavior:
1. keep build green (`python3 x.py build`)
2. update related docs under `docs/english` and `docs/korean`
3. keep module boundaries clean (no renderer leakage into unrelated layers)

## License
See `LICENSE`.
