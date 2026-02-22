# Core Module - Detailed Guide (Design Freeze)

Core is the lowest runtime layer. These boundaries are fixed.

1. Core provides execution/runtime infrastructure.
2. Graphics/Physics/UI/Runtime are built on top of Core only.
3. Scene is spatial/level orchestration. ECS is a data model and must live in Core/ECS.
4. Core must not depend on higher-level module types.

## 1. Current status summary
- Existing Core files: `Engine/Core/Logger.h`, `Engine/Core/Window.h`, `Engine/Core/Window.cpp`, `Engine/Core/RexMath.h`, `Engine/Core/Scene.h`, `Engine/Core/Components.h`
- Missing critical areas: Execution loop, Job system, Memory allocators, ECS separation, Module loader, Reflection, Global event bus, Time system, Resource handles, Crash/profiler diagnostics

## 2. Subsystems that must exist in Core

### 2.1 Execution Layer
- Responsibility:
Own Init -> Update -> Render -> Shutdown lifecycle
- Required:
EngineLoop, FrameContext, FixedStepAccumulator, PhaseScheduler
- Required phases:
PreUpdate, Update, PostUpdate, PreRender, Render, PostRender
- Acceptance:
Supports both fixed and variable timestep, with tested deterministic phase ordering.

### 2.2 Job System
- Responsibility:
Multicore scheduling and synchronization
- Required:
ThreadPool, TaskGraph, WorkStealingQueue, JobHandle
- Required primitives:
Mutex, Spinlock, Barrier, Condition
- Acceptance:
Deterministic frame-level output under parallel execution, with deadlock/starvation tests.

### 2.3 Memory System
- Responsibility:
Allocator abstraction and frame/pool/linear memory policies
- Required:
`IAllocator`, FrameAllocator, PoolAllocator, LinearAllocator, Arena
- Acceptance:
Measured reduction of allocations and fragmentation under heavy scene churn.

### 2.4 ECS (Core/ECS)
- Responsibility:
Entity/component storage and system scheduling
- Required:
Entity ID system, Component storage (Sparse Set or Archetype), SystemScheduler, World, Query
- Rule:
Move ECS storage logic out of `Scene` into `Core/ECS`.
- Acceptance:
Query performance and system order are covered by automated tests.

### 2.5 Module System
- Responsibility:
Dynamic loading, ABI compatibility, versioning, dependency resolution
- Required:
ModuleLoader, ModuleRegistry, ABI contract, Version checker, Dependency resolver
- Acceptance:
Module load failures must not crash the entire engine process.

### 2.6 Reflection System
- Responsibility:
Type/property metadata and serialization hooks
- Required:
TypeRegistry, PropertyMetadata, Runtime type lookup, Serialization hooks
- Acceptance:
Inspector UI can build editable fields from metadata without hardcoded field tables.

### 2.7 Global Event System
- Responsibility:
Engine-wide event publication/subscription
- Required:
EventBus, AsyncEventQueue, Lifecycle events
- Rule:
Separate from UI routed-event system.
- Acceptance:
Sync/async ordering rules are documented and test-covered.

### 2.8 Time System
- Responsibility:
`deltaTime`, `fixedDeltaTime`, `timeScale`, profiling clocks
- Required:
EngineClock, TimeState, ScopedTimer
- Acceptance:
Pause and time-scale changes remain consistent across physics/render update paths.

### 2.9 Resource Handle System
- Responsibility:
ID/generation-based references instead of raw pointers
- Required:
StrongHandle, WeakHandle, HandleAllocator, generation validation
- Acceptance:
Use-after-free is detected as invalid handle access at runtime.

### 2.10 Logging and Diagnostics
- Responsibility:
Logging, assertions, crash handling, profiling hooks
- Required:
Logger, Assert macros, Crash handler, Profiler hooks
- Acceptance:
Release builds still emit actionable fatal diagnostics and stack traces.

### 2.11 Platform Abstraction
- Responsibility:
OS, file IO, windowing, input abstraction
- Required:
Window, FileSystem, OS abstraction, Input abstraction
- Rule:
Platform-specific code remains isolated under Core/Platform.

## 3. Recommended folder layout
```text
Engine/Core/
  Execution/
    EngineLoop.h
    FrameContext.h
    PhaseScheduler.h
  Job/
    ThreadPool.h
    TaskGraph.h
    WorkStealingQueue.h
    SyncPrimitives.h
  Memory/
    IAllocator.h
    FrameAllocator.h
    PoolAllocator.h
    LinearAllocator.h
    Arena.h
  ECS/
    Entity.h
    ComponentStorage.h
    Query.h
    World.h
    SystemScheduler.h
  Module/
    ModuleLoader.h
    ModuleRegistry.h
    ModuleABI.h
  Reflection/
    TypeRegistry.h
    PropertyMetadata.h
    ReflectionMacros.h
  Event/
    EventBus.h
    AsyncEventQueue.h
    EngineEvents.h
  Time/
    EngineClock.h
    TimeState.h
  Resource/
    Handle.h
    HandlePool.h
  Diagnostics/
    Logger.h
    Assert.h
    CrashHandler.h
    ProfilerHooks.h
  Platform/
    Window.h
    FileSystem.h
    OS.h
    Input.h
  Math/
    RexMath.h
```

## 4. Migration policy from current code
- `Engine/Core/Scene.h`:
Move ECS storage/query internals into `Engine/Core/ECS/`; keep Scene as level/spatial orchestration.
- `Engine/Core/Components.h`:
Keep component schemas, but move storage and query ownership to ECS.
- `Engine/Core/Window.h`:
Relocate into Platform abstraction and split input abstraction from window lifecycle.
- `Engine/Core/Logger.h`:
Move under Diagnostics and integrate with assert/crash/profiler APIs.

## 5. Implementation priority
1. Execution plus Time
2. Job plus Memory
3. ECS extraction
4. Event plus Resource Handles
5. Module plus Reflection
6. Platform cleanup
7. Diagnostics expansion

## 6. Definition of Done
- Core standalone test targets validate Execution/Job/Memory/ECS/Event/Time.
- Runtime and Editor boot/update/shutdown through Core public APIs only.
- Scene has no direct dependency on ECS storage internals.
- Static dependency analysis reports zero upward references from Core.
