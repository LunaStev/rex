# Rex Editor Integrated Production Environment (AAA-Grade) Design

## 1. High-Level Goal
Evolve Rex Editor from a simple editing tool into a production platform that drives the full engine content pipeline.

Primary responsibilities:
- Scene and level authoring
- Entity/component editing
- Asset import/reimport and dependency tracking
- Debugging/profiling
- Build/package integration
- Scripting and plugin extensibility

Non-goals:
- Superficial UI cloning of another editor
- Monolithic implementation with hard coupling

## 2. Layered Architecture
```text
Editor Application Layer
  - EditorApp, Session, Workspace, layout persistence
          ↓
Editor Framework Layer
  - StateStore, CommandBus, SelectionManager, TransactionSystem
  - Docking/layout, shortcut contexts, registries
          ↓
Tool Modules Layer
  - Viewport/Outliner/Details/ContentBrowser/Log/Profiler/Settings
  - Gizmo + Selection/Move/Rotate/Scale tools
  - Asset import/reimport/thumbnail/dependency
          ↓
Engine Runtime Layer
  - Core(ECS/Execution/Time/Job), Graphics, Physics, UI runtime
```

Dependency rules:
- Strict top -> down dependency direction
- Tool modules use only public engine runtime APIs
- Model mutations must go through CommandBus + transactions

## 3. Operating Model

### 3.1 Editor Loop
- Init: project/modules/panels/layout restore
- Tick:
  - collect input
  - process commands
  - update tools
  - render engine scene viewport
  - render UI
- Shutdown: dirty check, auto-save, persist layout/settings

### 3.2 State Management
- `EditorStateStore`: single source of truth
- `SelectionManager`: selected entity/component/asset
- `TransactionSystem`: undo/redo transaction boundaries
- `EditorCommandBus`: only mutation path

Rule:
- UI widgets must not modify engine domain objects directly.

## 4. Required Panels
- Viewport Panel
- World Outliner
- Details Panel
- Content Browser
- Toolbar/Command Menu
- Output Log/Console
- Project Settings
- Modes Panel

Shared requirements:
- Dockable and detachable
- Layout save/restore
- Focus/context-aware shortcuts

## 5. Entity/Component Integration
- Actor/Entity create/delete
- Component attach/detach/reorder
- Custom component registration
- Reflection/metadata-driven property grid
- Metadata controls: range/step/tooltip/read-only

## 6. Gizmo System
- Translate/Rotate/Scale
- Local/World space switch
- Pivot edit
- Grid/rotation/scale snapping
- Multi-selection transforms

## 7. Workflow Features
- Full undo/redo integration
- Multi-selection batch edits
- Drag-drop assets to scene
- Prefab/Blueprint extension hook (optional)
- Scene save, auto-save, dirty-state indicators

## 8. Asset System
- `AssetRegistry`: GUID/path/type/metadata index
- `DependencyGraph`: reference and rebuild impact tracking
- `ImportPipeline`: format-specific importers + reimport tracking
- `ThumbnailService`: background thumbnail queue

## 9. Extensibility Model
- `IEditorModule` dynamic registration interface
- Registerable extension points:
  - panels
  - tools
  - commands
  - shortcuts
  - inspector widgets
- Module failure isolation required

## 10. Debugging and Profiling
- Real-time FPS and CPU/GPU frame time graphs
- Entity inspection
- Memory usage panel
- Filterable logging system

## 11. Performance Targets
- Responsive outliner at 100k entities
- No UI freeze while loading 1k+ assets
- Async background tasks:
  - asset scan/import
  - thumbnail generation
  - selected query/build tasks
- Explicit UI thread vs engine thread separation

## 12. Recommended Module Layout
```text
Engine/Editor/
  Core/
  Panels/
  Gizmo/
  Tools/
  Asset/
  Debug/
  Plugin/
```

## 13. Phased Delivery
- Phase A: framework core + minimum required panels
- Phase B: workflow completion (gizmo/tools/assets/undo-redo)
- Phase C: extension + diagnostics
- Phase D: large-scale performance

## 14. Definition of Done
- End-to-end: scene create -> entity add -> component edit -> save
- Accurate gizmo transforms
- Consistent undo/redo
- Stable docking layout restore
- No freeze with 1000+ assets loading
- Plugin-added panels run without core edits

