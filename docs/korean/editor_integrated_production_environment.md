# Rex Editor 통합 제작 환경(AAA급) 설계서

## 1. 상위 목표
Rex Editor를 단순 편집 툴이 아니라 엔진 제작 파이프라인의 중심 플랫폼으로 확장한다.

핵심 역할:
- Scene/Level 제작
- Entity/Component 기반 편집
- Asset Import/Reimport 및 의존성 추적
- 디버깅/프로파일링
- 빌드/패키징 파이프라인 연계
- 스크립트 및 플러그인 확장

비목표:
- 특정 상용 에디터 UI 외형 복제
- 단일 모놀리식 구현

## 2. 레이어 아키텍처
```text
Editor Application Layer
  - EditorApp, Session, Workspace, Layout persistence
  - 프로젝트 열기/닫기, 모드 전환, 실행 루프
          ↓
Editor Framework Layer
  - EditorStateStore, EditorCommandBus, SelectionManager, TransactionSystem
  - Docking/Layout, Shortcut contexts, Tool/Panel registries
          ↓
Tool Modules Layer
  - Viewport/Outliner/Details/ContentBrowser/Log/Profiler/ProjectSettings
  - Gizmo + Selection/Move/Rotate/Scale tools
  - Asset import/reimport/thumbnail/dependency
          ↓
Engine Runtime Layer
  - Core(ECS/Execution/Time/Job), Graphics, Physics, UI runtime
```

의존성 규칙:
- 상위 -> 하위 단방향
- Tool은 Engine 내부 구현이 아닌 공개 Runtime API만 사용
- EditorState 변경은 CommandBus/Transaction을 통해서만 수행

## 3. 운영 모델

### 3.1 에디터 루프
- Init: 프로젝트/모듈/패널 복원
- Tick:
  - 입력 수집
  - Command 처리
  - Tool 업데이트
  - Engine Scene Viewport 렌더
  - UI 렌더
- Shutdown: 더티 체크, 자동 저장, 레이아웃/설정 저장

### 3.2 상태 관리
- `EditorStateStore`: 단일 상태 소스
- `SelectionManager`: 현재 선택(엔티티/컴포넌트/애셋)
- `TransactionSystem`: Undo/Redo 트랜잭션 경계
- `EditorCommandBus`: 변경 유일 경로

규칙:
- UI 위젯이 모델 직접 수정 금지
- Property 변경은 항상 Command 생성 후 실행

## 4. 필수 패널 체계

### 4.1 Viewport Panel
- Engine Renderer 직접 사용
- Gizmo 조작, 카메라, 선택
- 디버그 오버레이: FPS/frametime/selection bounds
- 렌더 모드: Lit/Wireframe/Lighting-only/PostProcess toggle

### 4.2 World Outliner
- 계층 트리 + 검색 + 필터
- 100k 엔티티 대비 Virtualized Tree
- 다중 선택/배치 연산 지원

### 4.3 Details Panel
- Reflection/Metadata 기반 Property Grid 자동 구성
- 커스텀 Property Editor 등록 가능
- Range/Step/Tooltip/ReadOnly 등 메타데이터 반영

### 4.4 Content Browser
- Asset Registry 기반 가상 폴더 뷰
- 타입/태그/경로 필터
- 썸네일 비동기 생성
- 드래그앤드롭으로 Scene 배치

### 4.5 Toolbar / Command Menu
- 공용 명령 노출 + 단축키 힌트
- 모드 전환 및 빌드/실행 컨트롤

### 4.6 Output Log / Console
- 로그 레벨 필터, 카테고리 필터
- 명령 콘솔(선택)

### 4.7 Project Settings
- 프로젝트/렌더/입력/빌드 설정 편집
- 설정 변경 트랜잭션 기록

### 4.8 Modes Panel
- Select/Landscape/Modeling/Animation 등 모드 진입점
- 모드별 도구/단축키 컨텍스트 분리

공통 요구:
- Dock/Undock
- Layout Save/Restore
- 포커스 기반 단축키 컨텍스트

## 5. 엔티티/컴포넌트 통합
- Actor/Entity 생성/삭제
- Component attach/detach/reorder
- Custom Component 등록(플러그인)
- Reflection으로 Details 자동 생성
- Metadata 기반 UI 제약/설명 적용

## 6. Gizmo 시스템
- Translate/Rotate/Scale
- Local/World 축 전환
- Pivot 편집
- Grid/Rotation/Scale Snap
- Multi-selection 변환

정확성 요구:
- 월드/로컬 변환 오차 누적 최소화
- Undo/Redo 왕복 시 완전 동일 상태 재현

## 7. 워크플로우 기능
- 완전한 Undo/Redo
- Multi-selection batch edit
- Asset drag & drop -> Scene instantiate
- Prefab/Blueprint(선택) 확장 포인트
- Scene 저장, Auto-save, Dirty badge

## 8. 애셋 시스템
- `AssetRegistry`: GUID/경로/타입/메타데이터 인덱스
- `DependencyGraph`: 참조 관계/재빌드 영향 추적
- `ImportPipeline`: 포맷별 importer + reimport tracking
- `ThumbnailService`: 백그라운드 썸네일 큐

## 9. 플러그인/확장 모델
- `IEditorModule` 인터페이스 기반 동적 등록
- 등록 대상:
  - Panel
  - Tool
  - Command
  - Shortcut
  - Inspector widget
- 모듈 실패 격리(로딩 실패 시 에디터 다운 금지)

## 10. 디버깅/프로파일링
- 실시간 FPS + CPU/GPU frametime 그래프
- 선택 엔티티 인스펙션
- 메모리 사용량 뷰
- 로그 필터 시스템

## 11. 성능 목표
- 100k 엔티티 Outliner 반응성 유지
- 1k+ asset 로딩 시 UI thread freeze 금지
- 비동기 작업:
  - Asset scan/import
  - Thumbnail generation
  - 일부 query/build task
- UI thread와 Engine thread 경계 분리

## 12. 권장 코드 구조
```text
Engine/Editor/
  Core/
    EditorApp.h
    EditorSession.h
    EditorWorkspace.h
    EditorState.h
    EditorStateStore.h
    EditorCommandBus.h
    SelectionManager.h
    TransactionSystem.h
    LayoutService.h
  Panels/
    ViewportPanel.h
    OutlinerPanel.h
    DetailsPanel.h
    ContentBrowserPanel.h
    ToolbarPanel.h
    OutputLogPanel.h
    ProjectSettingsPanel.h
    ModesPanel.h
  Gizmo/
    TransformGizmo.h
  Tools/
    IEditorTool.h
    SelectionTool.h
    MoveTool.h
    RotateTool.h
    ScaleTool.h
  Asset/
    AssetRegistry.h
    DependencyGraph.h
    ImportPipeline.h
    ThumbnailService.h
  Debug/
    ProfilerPanel.h
    LogPanel.h
    MemoryPanel.h
  Plugin/
    IEditorModule.h
    EditorModuleManager.h
    EditorToolRegistry.h
    EditorPanelRegistry.h
    InspectorWidgetRegistry.h
    CommandRegistry.h
    ShortcutMap.h
```

## 13. 단계별 구현 전략

### Phase A: Core 프레임워크 고정
- EditorStateStore, CommandBus, SelectionManager, TransactionSystem
- Dock layout persistence
- 최소 패널(Viewport/Outliner/Details/ContentBrowser)

### Phase B: 제작 워크플로우 완성
- Gizmo + Tool contexts
- Asset registry/import/reimport + thumbnail worker
- Undo/Redo 전면 연동

### Phase C: 확장성과 진단
- Plugin module manager
- Custom inspector/tool/panel 등록
- Profiler/Memory/Log 고도화

### Phase D: 대규모 성능
- Virtualized outliner
- Async asset pipeline
- UI/Engine thread 협업 최적화

## 14. 수용 기준(Definition of Done)
- Scene 생성 -> Entity 추가 -> Component 편집 -> 저장 end-to-end 동작
- Gizmo 변환 정확성(로컬/월드/스냅) 검증
- Undo/Redo 일관성 유지
- Dock 레이아웃 저장/복원 안정성
- 1000+ asset 로드 중 UI freeze 없음
- Plugin으로 신규 패널 등록/실행 가능

