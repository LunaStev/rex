# Rex Engine Roadmap (Internal + External)

## 1. Recently Completed
### Rendering
- Deferred pipeline migration
- Render-graph style pass sequencing
- PBR Cook-Torrance shading path
- Cascaded directional shadow atlas path
- HDR + SSAO + Bloom + ACES tone mapping chain
- runtime stress scene integration (`Engine/Runtime/runtime_main.cpp`)

### Physics
- Rust physics core integration through C++ bridge
- quaternion dynamics and constraint flow active through runtime path

### Editor/UI
- Qt removal completed
- SDL + RexUI editor path stabilized
- next-gen industrial RexUI architecture skeleton fixed (design-first)
- integrated editor platform skeleton added under `Engine/Editor/`
- AAA editor architecture spec added: `docs/english/editor_integrated_production_environment.md`

## 2. Near-Term (Graphics)
### Goal
Close quality/performance gaps after deferred migration.

### Work
- Forward+ tile/cluster light culling
- better render-target aliasing/reuse in render graph
- improved cascade stability (texel snapping and split tuning)
- optional soft shadow quality tiers

### Exit Criteria
- stable frame time with large light counts
- reduced shadow shimmering in camera motion
- no major visual regression from current deferred path

## 3. Mid-Term (Graphics + Runtime)
### Goal
Complete physically-based environment pipeline and improve cinematic quality.

### Work
- full IBL workflow (irradiance + prefiltered env + BRDF LUT)
- color grading LUT pipeline
- optional advanced effects (TAA/DOF/motion blur) with quality gates
- GPU profiler markers and frame breakdown tooling

### Exit Criteria
- consistent PBR response under environment lighting
- measurable and documented per-pass GPU cost
- runtime quality presets for low/mid/high targets

## 4. UI Framework Track
### Goal
Move from architecture skeleton to production-capable framework modules.

### Work
- StateStore/Binding/Diff core loop
- DockManager + Undo/Redo integration
- virtualization for large hierarchy views

### Exit Criteria
- stable editor interaction on large scenes
- deterministic UI state transitions under undo/redo

## 5. Editor Platform Track
### Goal
Move from demo/editor shell to production-grade integrated authoring environment.

### Work
- Editor Application/Framework/Tool layering hardening
- Viewport/Outliner/Details/Content Browser full workflow integration
- Transaction + Selection + Command-only mutation path
- Plugin module loading for custom panels/tools/inspectors/commands
- async asset pipeline + thumbnail background workers

### Exit Criteria
- end-to-end scene authoring flow is stable
- docking layouts persist reliably
- large hierarchy/assets remain responsive

## 6. Internal Quality Targets
- build and run checks remain green on every major subsystem update
- docs stay synchronized with implementation
- performance-sensitive paths always have measurable counters
