# RexUI Industrial Architecture Guide

This document defines how RexUI evolves from a basic widget set into an industrial-grade framework.

## 1. Why this is needed
As editor complexity grows, docking, state management, Undo/Redo, and large-list performance become core bottlenecks.

## 2. Targets
- functional/structural parity with legacy Qt editor behavior
- extensible architecture (plugins/custom widgets/backend swap)
- performance through diff updates, virtualization, and layout caching

## 3. Layer structure
- Core: widget/event/style/geometry contracts
- Framework: state, binding, commands, input, docking, animation, plugins
- Runtime: widget tree, diff, layout, render graph
- Renderer: `IRenderBackend` + OpenGL/Vulkan implementations
- Widgets: basic/editor widgets
- Integration: editor-domain bridge

## 4. Internal architectural rules
- interface-first
- one-way dependencies
- no direct model mutation from UI; use CommandBus
- no renderer-specific code leakage into upper layers

## 5. External usage model
- long-term declarative view + state binding API
- custom widgets registered via registry/factory
- panel/tool injection through plugin model

## 6. Core capability axes
- routed events (capture/bubble)
- StateStore + BindingEngine
- CommandBus + Undo/Redo
- DockManager
- timeline transitions
- virtualized list/tree

## 7. Performance principles
- subtree diff instead of full rebuild
- realize visible range only
- cache layout/draw intermediates
- optimize draw command batching

## 8. Reference
- execution lock and phased implementation: `rexui_framework_execution_lock.md`
- skeleton code root: `Engine/UI/RexUI/`
