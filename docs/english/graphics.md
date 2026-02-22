# Graphics Module - Detailed Guide

## 1. Responsibility
Graphics turns Scene data into rendered frames.

Main classes:
- `Renderer`
- `Mesh`
- `Model`
- `Shader`

## 2. Renderer API
```cpp
void render(Scene& scene,
            const Camera& camera,
            const Mat4& viewMatrix,
            const Vec3& viewPos,
            int targetWidth,
            int targetHeight,
            uint32_t backbufferFBO,
            const std::function<void(const Mat4&, const Mat4&)>& extraDraw = {});
```

Parameters:
- scene: ECS render source
- camera/viewMatrix/viewPos: view configuration
- dimensions: output size
- backbufferFBO: destination framebuffer
- extraDraw: custom debug overlay pass

## 3. Frame flow (conceptual)
1. prepare HDR target
2. scene mesh pass
3. tonemap pass
4. optional extra draw callback

## 4. External usage
### 4.1 Minimal sample
```cpp
rex::Renderer renderer;
rex::Scene scene;
rex::Camera cam;
rex::Vec3 camPos{0,2,-8};
rex::Mat4 view = rex::Mat4::lookAtLH(camPos, {0,0,0}, {0,1,0});

renderer.render(scene, cam, view, camPos, width, height, 0);
```

### 4.2 Add mesh
- use `Mesh::createCube()` for bootstrap geometry
- bind via `MeshRenderer`

## 5. Internal extension guide
### 5.1 Add a new render pass
- define pass ordering in `Renderer`
- manage resource lifetime explicitly
- expose debug toggle where possible

### 5.2 Shader change policy
- document uniform contract changes
- keep material parameter compatibility

## 6. Performance focus
- draw calls
- state switches
- buffer uploads
- HDR resource reallocations

## 7. Troubleshooting
- black frame: check GL init/context validity
- broken lighting: verify `Light` entities
- perf drop: inspect `extraDraw` state churn
