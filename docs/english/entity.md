# Entity/Component Operation Guide

Rex follows ECS: entities are IDs, and behavior lives in systems.

## 1. Basic pattern
### 1.1 Create entity
```cpp
rex::EntityId e = scene.createEntity();
scene.addComponent<rex::Transform>(e, rex::Vec3{0, 0, 0});
scene.addComponent<rex::MeshRenderer>(e, nullptr, cube, rex::Vec3{1,1,1});
```

### 1.2 Destroy entity
```cpp
scene.destroyEntity(e);
```

## 2. Practical archetypes
### 2.1 Dynamic box
- `Transform`
- `MeshRenderer`
- `RigidBodyComponent` (Dynamic)

### 2.2 Static ground
- `Transform` with large scale
- `MeshRenderer`
- `RigidBodyComponent` (Static)

### 2.3 Light entity
- `Transform`
- `Light`

## 3. External developer workflow
1. create entity
2. attach components
3. run system updates
4. render and inspect in editor

## 4. Internal extension workflow
### 4.1 Add new component
1. add struct to `Engine/Core/Components.h`
2. add system pass over `each<NewComponent>`
3. expose in editor inspector (if needed)

### 4.2 Add new system
- input: `Scene&`, `dt`
- output: component updates
- keep inter-system coupling minimal

## 5. Safe access rules
- `getComponent<T>` may return null
- do not keep long-lived stale entity caches

## 6. Common mistakes
- owning/freeing `RigidBodyComponent.internalBody` externally
- overwriting Transform at wrong sync stage, causing jitter
- implicit assumptions about component existence

## 7. Debug tips
- print selected entity ID
- log component presence checks
- compare transform before/after physics sync
