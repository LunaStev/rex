# RexUI Execution Lock (Design Freeze + Incremental Delivery)

This document enforces structure-first development and prevents all-at-once unstable implementation.

## 1. Scope
- internal engine developers
- editor/toolchain UI contributors
- architecture/performance reviewers

## 2. Fixed principles
1. freeze architecture first
2. create interfaces/folders/files first
3. lock TODO metadata first
4. implement incrementally by module phase

## 3. Current lock state
- skeleton path: `Engine/UI/RexUI/`
- file count: 54
- TODO count: 54
- each TODO includes:
  - unique ID
  - responsibility
  - requirements
  - dependencies
  - implementation phase
  - performance notes
  - test strategy

## 4. Phase order
- Phase A: Geometry, Widget interface, WidgetTree MVP
- Phase B: LayoutEngine, EventRouter MVP
- Phase C: StateStore, minimal Binding
- Phase D: DiffEngine, DrawCommand/RenderGraph
- Phase E: DockManager, Undo/Redo, Virtualization

## 5. Change-control rules
- no architecture drift during implementation
- no upward dependency from lower layers
- no direct model mutation from UI (CommandBus only)
- no renderer-specific leakage into framework/runtime

## 6. Code review checklist
- TODO ID and implementation scope alignment
- dependency direction compliance
- test coverage matches TODO test strategy
- measurable metrics in performance-sensitive paths

## 7. External communication policy
- when API changes, update `overview.md` and module docs together
- provide migration notes for breaking changes
