# Physics Module - Detailed Guide

## 1. Architecture
Physics runtime is split into:
- `RigidBody` (C++ handle exposed to gameplay/ECS)
- `PhysicsSystem` (ECS sync, fixed-step orchestration, API surface)
- Rust core (`Engine/Rust/physics_core`) accessed through FFI (`RustPhysicsFFI.h`)

`PhysicsWorld` C++ implementation has been retired in favor of the Rust core path.

## 2. Runtime Flow
`PhysicsSystem::update(scene, dt)` drives the simulation.

High-level flow:
1. sync ECS components to body handles
2. fixed-step accumulation
3. invoke Rust world stepping through FFI
4. apply joint/constraint state updates
5. write back pose/velocity to ECS components

## 3. Features in Current Path
- rigid body simulation (dynamic/kinematic/static)
- linear/angular velocity integration
- gravity, damping, friction/restitution parameters
- CCD-enabled path in runtime configuration
- distance joint registration and management
- scene raycast API (`PhysicsSystem::raycast`)

## 4. External Usage
### 4.1 Configure and update
```cpp
rex::PhysicsSystem physics;
physics.setGravity({0.0f, -9.81f, 0.0f});
physics.setSolverIterations(14, 6);
physics.setMaxSubSteps(8);

physics.update(scene, dt);
```

### 4.2 Add rigid body component
```cpp
auto& rb = scene.addComponent<rex::RigidBodyComponent>(entity, rex::BodyType::Dynamic, 1.0f);
rb.enableCCD = true;
rb.restitution = 0.2f;
rb.staticFriction = 0.7f;
rb.dynamicFriction = 0.5f;
```

### 4.3 Add distance joint
```cpp
rex::DistanceJointDesc j;
j.bodyA = rbA->internalBody;
j.bodyB = rbB->internalBody;
j.restLength = 1.8f;
j.stiffness = 0.8f;
j.damping = 0.2f;

int id = physics.addDistanceJoint(j);
```

Important: call `physics.update(scene, 0.0f)` once before creating joints if internal bodies are not created yet.

## 5. Tuning Tips
- jitter in stacks: increase solver iterations and tune damping
- tunneling: keep CCD enabled and increase max substeps
- explosive contacts: reduce restitution and impulse bursts
- unstable chains: lower stiffness or increase damping

## 6. Internal Extension Notes
- keep FFI boundary data-oriented and explicit
- avoid C++/Rust ownership ambiguity (Rust world owns simulation state)
- keep `RigidBody` API stable while changing internal Rust modules
- validate scene sync performance with large entity counts

## 7. Debug Checklist
- `RigidBodyComponent.internalBody` assigned before applying impulses/joints
- gravity/solver/substep settings are expected for test scene
- dt spikes are clamped by runtime loop
- raycast direction is normalized in callers
