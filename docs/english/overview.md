# Rex Engine Overview (Internal + External)

This documentation set targets both:
- external developers building runtime content/tools with Rex
- internal developers extending engine architecture and systems

## 1. Current Engine Snapshot
- Language/build: C++20 + CMake + `python3 x.py`
- Rendering: modular Deferred pipeline (PBR, CSM shadows, HDR, SSAO, Bloom, ACES tone mapping)
- Physics: Rust rigid-body core + C++ bridge (`PhysicsSystem`)
- UI/editor: SDL + RexUI (`rex-editor`), Qt removed
- editor architecture: AAA-grade integrated production environment skeleton in `Engine/Editor/`

## 2. Repository Layout
```text
Engine/
  Core/        # ECS, math, logger, window
  Graphics/    # modular renderer stack
  Physics/     # C++ bridge layer for Rust physics
  Rust/        # Rust crates (physics core)
  UI/          # legacy RexUI + next-gen framework skeleton
  Editor/      # integrated editor platform modules (core/panels/tools/asset/plugin)
  EditorRex/   # editor entry point
  Runtime/     # runtime sandbox entry point
docs/
  english/
  korean/
```

## 3. Build and Run
### 3.1 Build (recommended)
```bash
python3 x.py
```

### 3.2 Additional build commands
```bash
python3 x.py configure
python3 x.py build
python3 x.py run editor
python3 x.py run editor-legacy
python3 x.py run runtime
```

### 3.3 Direct execution
```bash
./build/rex-editor
./build/rex-editor-legacy
./build/rex-runtime
```

## 4. Runtime Entry Recommendation
Use `Engine/Runtime/runtime_main.cpp` as the baseline integration sample.

It demonstrates:
- ECS setup
- Rust physics update loop
- Deferred renderer integration
- many-light visual stress scene
- runtime post-process control

## 5. Runtime Controls Summary
- camera: `WASD + QE`, `Shift`, `RMB + mouse`
- physics gameplay: `LMB`, `Space`, `B`, `J`, `R`, `T`, `G`, `P`, `Delete`
- graphics tuning: `F1`, `F2`, `F3/F4`, `F5/F6`, `L`, `K`

## 6. Internal Development Rules
- Keep module boundaries explicit (Core/Graphics/Physics/UI contracts)
- Avoid upward dependency leaks from low-level modules
- Keep docs synchronized with behavior changes
- Verify with build before merge (`python3 x.py build`)

## 7. Where to Read Next
- rendering details: `docs/english/graphics.md`
- physics integration: `docs/english/physics.md`
- ECS and components: `docs/english/core.md`, `docs/english/entity.md`
- runtime/world flow: `docs/english/world.md`
- near/far plans: `docs/english/roadmap.md`
- AAA editor architecture: `docs/english/editor_integrated_production_environment.md`
