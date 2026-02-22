# World (= Scene) Operation Guide

In Rex, the world is represented by `Scene`.

## 1. Recommended update order
1. input processing
2. command application (UI/game)
3. physics update
4. rendering
5. UI overlay rendering

## 2. Multi-scene operation
Scene switching is managed by user code.

Recommended pattern:
- `std::unique_ptr<Scene> activeScene`
- swap pointer after load completion
- resync system caches (physics/UI)

## 3. Save/load strategy (recommended)
No built-in serializer yet. Recommended partition:
- entity list
- per-component data blobs
- version and migration metadata

## 4. Editor perspective
- hierarchy = scene entity projection
- inspector = selected entity component projection
- with Undo/Redo, mutate scene through commands only

## 5. Internal checklist
- clear system caches on scene disposal
- validate `internalBody` pointer lifetime
- reset selection state on scene replacement
