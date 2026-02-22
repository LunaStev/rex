# Input Handling Guide

Rex currently uses SDL event loops with explicit ownership per execution context.

## 1. Current structure
- gather events through SDL (`SDL_PollEvent`)
- forward UI input into `UIContext::dispatch*`
- process camera controls in editor loop

## 2. Default editor keymap
- movement: `W/A/S/D`
- vertical: `Space` / `Ctrl`
- look mode: hold RMB
- gizmo mode: `Q/W/E/R`
- delete: `Delete`

## 3. UI vs viewport conflict handling
Priority order:
1. focused text input
2. UI capture events
3. viewport navigation
4. global shortcuts

Practical rule:
- use `ui.dispatchKeyDown()` return value to decide if event is consumed
- keep `SDL_TEXTINPUT` independent from key down/up

## 4. External integration pattern
```cpp
while (SDL_PollEvent(&ev)) {
    // 1) feed UI first
    // 2) if not consumed, handle camera/game input
}
```

## 5. Internal roadmap
Converge into next-gen UI input abstraction:
- `Framework/Input/InputEvent.h`
- `Framework/Input/InputMap.h`
- `Framework/Input/InputRouter.h`

Goals:
- decouple physical input from logical actions
- context-aware input routing
- keyboard/mouse/gamepad unification

## 6. Test strategy
- ensure focused text widgets do not leak movement keys
- validate mouse capture boundaries
- stress key repeat behavior
