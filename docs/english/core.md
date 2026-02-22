# Core Module - Detailed Guide

Core is the foundational layer shared by all engine subsystems.

## 1. Main parts
- `Scene`: ECS store
- `Components`: common data schema
- `RexMath`: vector/matrix/quaternion math
- `Window`: SDL + OpenGL context wrapper
- `Logger`: formatted logging

## 2. Scene design
### 2.1 Data model
- `Scene` stores component pools by `std::type_index`
- each pool is `TypedComponentPool<T>`

### 2.2 Contract
- `createEntity()` allocates ID only
- meaning comes from component composition
- `destroyEntity()` removes ID from all pools

### 2.3 Iteration
```cpp
scene.each<rex::Transform>([&](rex::EntityId id, rex::Transform& t) {
    // system logic
});
```

Recommendations:
- avoid heavy allocations inside tight ECS loops
- stage writes if mutating multiple component sets

## 3. Components details
### 3.1 Transform
- position/rotation(deg)/scale
- `getMatrix()` builds transform matrix

### 3.2 MeshRenderer
- render source (`mesh` or `model`)
- includes PBR params

### 3.3 RigidBodyComponent
- physics parameters + internal body pointer
- pointer lifetime is managed by `PhysicsSystem`

### 3.4 Camera / Light
- Camera uses LH projection (`perspectiveLH`)
- Light supports Directional/Point

## 4. RexMath usage
### 4.1 Quaternion
- `Quat::fromEulerXYZ`, `toEulerXYZ`, `rotate`
- used by physics orientation integration

### 4.2 Mat4
- transforms: `translate`, `rotateX/Y/Z`, `scale`
- camera/projection: `lookAtLH`, `perspectiveLH`

### 4.3 Unit conventions
- editor transform rotation is degrees
- physics orientation math is radians at conversion boundaries

## 5. Window and Logger
### 5.1 Window
- creates SDL window and GL context
- `pollEvents`, `swapBuffers`, `setVSync`

### 5.2 Logger
- `std::format`-based
- `trace/info/warn/error`

## 6. External checklist
- can build a minimal app with Scene+Renderer+Window
- system update order follows documentation

## 7. Internal checklist
- assess downstream impact for Core API changes
- update docs and samples with signature changes
