# Graphics Module - Internal/External Developer Guide

## 1. Scope
The graphics module is now organized as a real-time rendering pipeline, not a single monolithic renderer.

Primary goals:
- visual fidelity upgrade (PBR + HDR + post process)
- clear module boundaries for maintainability
- scalable many-light rendering path

## 2. High-level Architecture
The runtime-facing class is still `rex::Renderer`, but it now forwards work to a modular deferred pipeline.

Primary flow:
1. `ShadowPass`
2. `GBufferPass`
3. `LightingPass`
4. `PostProcessPass`
5. `UIPass` (reserved integration point)

Relevant code:
- `Engine/Graphics/Renderer.h`
- `Engine/Graphics/Pipeline/DeferredPipeline.h`
- `Engine/Graphics/Core/RenderGraph.h`

## 3. Module Split
```text
Engine/Graphics/
  Core/
    RenderDevice.*
    RenderPass.h
    RenderGraph.*
    FrameBuffer.*
  Lighting/
    Light.h
    LightManager.*
    ShadowSystem.*
  Material/
    Material.h
    PBRMaterial.h
    ShaderVariant.h
  Pipeline/
    DeferredPipeline.*
    ForwardPipeline.*
    PostProcessPipeline.*
  PostProcess/
    SSAOPass.*
    BloomPass.*
    ToneMappingPass.*
  HDR/
    HDRBuffer.*
    ExposureController.*
  Culling/
    FrustumCuller.*
    LightCuller.*
```

## 4. Lighting Model
Supported `Light` types (`Engine/Core/Components.h`):
- Directional
- Point
- Spot
- Area

Each light supports:
- linear-space `color`
- `intensity`
- `castShadows`
- attenuation/range controls
- spot cone controls (`innerConeDeg`, `outerConeDeg`)
- `volumetric` flag (data path only; full volumetric pass not complete)

## 5. Shading Pipeline
### 5.1 GBuffer layout
- RT0: world position
- RT1: normal + roughness
- RT2: albedo + metallic
- RT3: AO

### 5.2 Lighting model
- Cook-Torrance BRDF
- GGX NDF + Smith geometry + Schlick Fresnel
- metallic/roughness workflow

### 5.3 HDR and color
- HDR lighting target
- SSAO and Bloom as separate passes
- ACES filmic tone mapping
- gamma correction to display output

## 6. Shadows
Directional shadows use cascaded shadow maps with an atlas layout.

Current implementation:
- 4 cascades
- atlas partitioning
- PCF sampling in lighting pass
- slope-aware depth bias

## 7. Runtime Integration
Minimal API remains:
```cpp
renderer.render(scene, camera, view, viewPos, width, height, backbufferFBO);
```

Post process runtime control:
```cpp
auto& pp = renderer.deferredPipeline().postProcess().settings();
pp.enableBloom = true;
pp.autoExposure = true;
pp.exposure = 1.15f;
pp.bloomStrength = 0.14f;
```

## 8. Runtime Sandbox Profile
`Engine/Runtime/runtime_main.cpp` is configured to actively stress this pipeline:
- mixed light types
- shadowed directional key light
- animated point/spot lights
- 128 stress point lights
- interactive post-process toggles (bloom/exposure)

This file is the recommended baseline for both rendering and physics integration tests.

## 9. Performance Notes
Already implemented:
- frustum-side visible renderable filtering
- CPU-side light ranking/culling
- render-target reuse via persistent framebuffer objects

Planned next:
- Forward+ tile/cluster GPU light culling
- stronger render-graph resource aliasing
- expanded occlusion strategy

## 10. Troubleshooting
- Black frame:
  - verify GL function loading (`Engine/Graphics/GLInternal.h`)
  - verify FBO completeness (`FrameBuffer`)
- No shadows:
  - ensure a directional light has `castShadows = true`
- Flat look:
  - check exposure and bloom settings
  - verify roughness/metallic values in `MeshRenderer`
- Perf drop with many lights:
  - reduce stress light count or range
  - disable animation for diagnostic (`K` in runtime sandbox)
