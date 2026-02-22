# Rex Engine Developer Documentation (Internal + External)

This documentation set is intended for both:
- **External developers** who build games/tools on top of Rex
- **Internal developers** who extend and maintain the engine itself

## 1. Audience
- External:
  - needs practical usage flow, build/run commands, API examples
- Internal:
  - needs architecture boundaries, development rules, quality gates

## 2. Current engine snapshot
- Language/build: C++20 + CMake
- Rendering: OpenGL pipeline (`Renderer`)
- Physics: rigid-body simulation (`PhysicsWorld`, `PhysicsSystem`)
- UI: legacy RexUI plus next-gen industrial RexUI skeleton
- Editor: SDL + RexUI (`rex-editor`), Qt removed

## 3. Repository layout
```text
Engine/
  Core/        # ECS, math, window/logger
  Graphics/    # renderer, shader, mesh/model
  Physics/     # world, rigid body, ECS bridge
  UI/          # legacy RexUI + next-gen skeleton
  EditorRex/   # editor entry
  Runtime/     # runtime sample entry
docs/
  korean/
  english/
```

## 4. Required dependencies
- OpenGL
- SDL2
- Freetype
- C++20 toolchain

Fedora-like example:
```bash
sudo dnf install gcc-c++ cmake SDL2-devel freetype-devel mesa-libGL-devel
```

## 5. Build and run
### 5.1 Build
```bash
cmake -S . -B build
cmake --build build -j4
```

### 5.2 Run
```bash
./build/rex-editor
./build/rex-runtime
```

## 6. External developer quick start
### 6.1 Minimal runtime flow
1. Create `Window`
2. Create `Scene`
3. Add `Transform` + `MeshRenderer`
4. Update physics (`PhysicsSystem`)
5. Render (`Renderer`)

Use `Engine/Runtime/runtime_main.cpp` as the baseline.

### 6.2 Editor usage highlights
- Add/Delete entities from top controls
- select from hierarchy
- edit transform in details and apply
- RMB + WASD camera navigation

## 7. Internal developer rules
### 7.1 Layer boundaries
- Core should not depend on upper-layer policies
- backend-specific code stays under Renderer
- UI mutates domain state through command/state flow

### 7.2 Keep docs in sync
When changing these areas, update matching docs:
- physics logic -> `physics.md`
- render pipeline -> `graphics.md`
- UI architecture -> `rexui_*.md`

### 7.3 Quality gate
- build must pass
- no behavioral regressions
- docs and code must match
- performance-critical paths must remain measurable

## 8. Troubleshooting
- startup exit: inspect SDL/GL init logs (`Window`)
- missing UI text: verify `Engine/UI/Fonts/Roboto-Regular.ttf`
- physics jitter/tunneling: increase solver/substeps, verify CCD

## 9. Related docs
- `core.md`, `entity.md`, `graphics.md`, `physics.md`, `input.md`, `world.md`
- `roadmap.md`
- `rexui_slate_grade_architecture.md`
- `rexui_framework_execution_lock.md`
