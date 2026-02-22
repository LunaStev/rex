# Core 모듈 상세 가이드 (Design Freeze)

Core는 엔진의 최하위 실행 계층이며, 다음 규칙을 고정한다.

1. Core는 정책과 런타임 인프라를 제공한다.
2. Graphics/Physics/UI/Runtime는 Core 위에서만 동작한다.
3. Scene은 공간/레벨 개념이다. ECS는 데이터 구조이며 Core/ECS로 분리한다.
4. 상위 모듈의 타입을 Core가 참조하지 않는다.

## 1. 현재 상태 요약
- 현재 구현된 Core 파일: `Engine/Core/Logger.h`, `Engine/Core/Window.h`, `Engine/Core/Window.cpp`, `Engine/Core/RexMath.h`, `Engine/Core/Scene.h`, `Engine/Core/Components.h`
- 현재 부족한 영역: Execution loop, Job system, Memory allocators, ECS 분리, Module loader, Reflection, Global event bus, Time system, Resource handle, Crash/profiler diagnostics

## 2. Core에 반드시 포함할 서브시스템

### 2.1 Execution Layer
- 책임:
Init -> Update -> Render -> Shutdown 프레임 수명주기 통제
- 필수 요소:
EngineLoop, FrameContext, FixedStepAccumulator, PhaseScheduler
- 프레임 페이즈:
PreUpdate, Update, PostUpdate, PreRender, Render, PostRender
- 수용 기준:
Fixed timestep과 variable timestep을 동시에 지원하고, 페이즈 실행 순서가 테스트로 고정되어야 한다.

### 2.2 Job System
- 책임:
멀티코어 작업 스케줄링과 동기화
- 필수 요소:
ThreadPool, TaskGraph, WorkStealingQueue, JobHandle
- 동기화 프리미티브:
Mutex, Spinlock, Barrier, Condition
- 수용 기준:
단일 프레임 내 병렬 작업 실행 결과가 결정적이어야 하며, 교착/기아 방지 테스트를 통과해야 한다.

### 2.3 Memory System
- 책임:
할당 정책 추상화와 프레임/풀/선형 메모리 관리
- 필수 요소:
`IAllocator`, FrameAllocator, PoolAllocator, LinearAllocator, Arena
- 수용 기준:
대량 스폰/삭제 시 일반 `new/delete` 대비 할당 횟수와 파편화가 계측으로 개선되어야 한다.

### 2.4 ECS (Core/ECS)
- 책임:
엔티티/컴포넌트 저장과 시스템 실행 계획
- 필수 요소:
Entity ID system, Component storage(Sparse Set 또는 Archetype), SystemScheduler, World, Query
- 규칙:
`Scene`에서 ECS 저장소 로직을 제거하고 `Core/ECS`로 이관한다.
- 수용 기준:
Query 성능과 시스템 순서가 자동 테스트로 고정되어야 한다.

### 2.5 Module System
- 책임:
동적 모듈 로딩, ABI/버전 호환, 의존성 해결
- 필수 요소:
ModuleLoader, ModuleRegistry, ABI contract, Version checker, Dependency resolver
- 수용 기준:
모듈 실패 로드가 엔진 전체 크래시로 전파되지 않아야 한다.

### 2.6 Reflection System
- 책임:
타입/프로퍼티 메타데이터와 직렬화 훅 제공
- 필수 요소:
TypeRegistry, PropertyMetadata, Runtime type lookup, Serialization hooks
- 수용 기준:
Inspector가 문자열 하드코딩 없이 메타데이터 기반으로 필드를 구성할 수 있어야 한다.

### 2.7 Global Event System
- 책임:
엔진 전역 이벤트 발행/구독
- 필수 요소:
EventBus, AsyncEventQueue, Lifecycle events
- 규칙:
UI 이벤트 시스템과 분리
- 수용 기준:
동기/비동기 이벤트 처리 순서 규약이 문서와 테스트로 고정되어야 한다.

### 2.8 Time System
- 책임:
`deltaTime`, `fixedDeltaTime`, `timeScale`, profiler clock 관리
- 필수 요소:
EngineClock, TimeState, ScopedTimer
- 수용 기준:
슬로모션과 일시정지 시뮬레이션이 물리/렌더와 일관되게 동작해야 한다.

### 2.9 Resource Handle System
- 책임:
리소스 참조를 포인터가 아닌 핸들로 통일
- 필수 요소:
StrongHandle, WeakHandle, HandleAllocator, generation-based validation
- 수용 기준:
해제된 리소스 접근이 즉시 무효 핸들로 검출되어야 한다.

### 2.10 Logging & Diagnostics
- 책임:
로그/어설션/크래시/프로파일링 후크
- 필수 요소:
Logger, Assert macros, Crash handler, Profiler hooks
- 수용 기준:
릴리즈 빌드에서 치명 에러 리포트와 콜스택 출력 경로가 보장되어야 한다.

### 2.11 Platform Abstraction
- 책임:
OS/입출력/윈도우/입력 공통 API
- 필수 요소:
Window, FileSystem, OS abstraction, Input abstraction
- 규칙:
플랫폼 종속 코드는 Core/Platform 하위로 격리한다.

## 3. 권장 폴더 구조
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

## 4. 기존 코드 이관 원칙
- `Engine/Core/Scene.h`:
ECS 저장소 로직을 `Engine/Core/ECS/`로 이동하고, Scene은 월드/레벨 관리 API만 남긴다.
- `Engine/Core/Components.h`:
컴포넌트 타입 정의는 유지하되 저장/쿼리 책임은 ECS로 이전한다.
- `Engine/Core/Window.h`:
Platform 하위로 이동하고 입력 추상화를 분리한다.
- `Engine/Core/Logger.h`:
Diagnostics로 이동하고 Assert/Crash/Profiler와 통합한다.

## 5. 구현 우선순위 (권장)
1. Execution + Time
2. Job + Memory
3. ECS 분리
4. Event + Resource Handle
5. Module + Reflection
6. Platform 정리
7. Diagnostics 확장

## 6. Definition of Done
- Core 독립 테스트 타겟에서 Execution/Job/Memory/ECS/Event/Time가 모두 단위 테스트를 통과한다.
- Runtime과 Editor가 Core 공개 API만으로 초기화/루프/종료를 수행한다.
- Scene이 ECS 저장 세부구현에 직접 의존하지 않는다.
- 상위 모듈 역참조가 정적 분석에서 0건이어야 한다.
