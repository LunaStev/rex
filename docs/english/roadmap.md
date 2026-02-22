# Rex Engine Roadmap (Internal + External)

## 1. Completed
### Engine
- ECS baseline
- OpenGL renderer
- physics upgrades (Quat, CCD, Dynamic AABB Tree, 2~4 manifold)

### Editor
- Qt removed
- SDL + RexUI editor migration completed

### UI framework
- industrial RexUI skeleton created (Design Freeze + Interface-first)

## 2. Short-term (Phase A/B)
### Goal
- deliver minimal next-gen RexUI runtime loop

### Work
- implement Core geometry/widget interfaces
- implement WidgetTree MVP
- implement base LayoutEngine/EventRouter

### Exit criteria
- basic widget tree render + event propagation works
- minimal unit tests pass

## 3. Mid-term (Phase C/D)
### Goal
- establish state-driven UI pipeline

### Work
- UIStateStore + minimal BindingEngine
- DiffEngine + DrawCommandBuilder
- production OpenGL backend (`IRenderBackend`)

### Exit criteria
- state change -> minimal patch -> rendered output
- measurable perf counters exposed

## 4. Long-term (Phase E+)
### Goal
- complete industrial editor capabilities

### Work
- DockManager
- full Undo/Redo integration
- virtualized hierarchy
- Vulkan backend
- plugin SDK

### Exit criteria
- stable editing on large scenes
- backend swap with minimal upper-layer changes

## 5. External impact
- short-term: mostly stable API, better docs
- mid-term: partial standardization in UI/input contracts
- long-term: extensible plugin/toolchain interfaces

## 6. Internal quality targets
- CI build/test automation
- regression checklist operation
- long-run stability (resource and memory leak control)
