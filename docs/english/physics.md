# Physics Module - Detailed Guide

## 1. Components
- `RigidBody`: body state and force/impulse APIs
- `PhysicsWorld`: simulation, collision, constraints, CCD
- `PhysicsSystem`: ECS sync bridge

## 2. Simulation pipeline
`PhysicsWorld::step(dt)` runs an internal fixed-step loop.

Per substep (high level):
1. capture start state
2. integrate linear/angular motion + damping
3. update broadphase (Dynamic AABB Tree)
4. apply TOI CCD
5. narrowphase (SAT OBB + manifold build)
6. warmstart + velocity solve
7. position solve
8. update caches/sleep states

## 3. Current feature set
- quaternion rotational dynamics
- world inverse inertia application
- Dynamic AABB Tree broadphase
- contact manifold with 2~4 points
- manifold warmstart/cache
- Distance Joint
- TOI-based CCD (swept-sphere approximation)
- OBB raycast

## 4. External usage
### 4.1 Component setup
```cpp
scene.addComponent<rex::RigidBodyComponent>(entity, rex::BodyType::Dynamic, 1.0f);
```

### 4.2 System update
```cpp
rex::PhysicsSystem physics;
physics.getWorld().setSolverIterations(12, 5);
physics.getWorld().setMaxSubSteps(8);

physics.update(scene, dt);
```

### 4.3 Joint setup
- run `physics.update(scene, 0.0f)` once to create internal bodies
- access `RigidBodyComponent.internalBody`
- create and register `DistanceJointDesc`

## 5. Tuning guide
- reduce jitter: increase solver iterations
- reduce tunneling: increase substeps + enable CCD
- reduce bounce instability: adjust restitution and threshold
- stack stability: review friction and sleep params

## 6. Internal extension guide
### 6.1 New collider shape
- keep broadphase proxy strategy compatible
- preserve manifold key and warmstart compatibility

### 6.2 New constraint type
- follow prepare/warmstart/velocity/position phases
- integrate into existing solver loops

## 7. Debug checklist
- body type/mass/invMass consistency
- degree/radian boundary correctness
- CCD flag state
- sleep transitions causing unexpected freeze
