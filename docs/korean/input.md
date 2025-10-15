# Input Module
`Input`모듈은 키보드 입력을 관리하는 시스템이다.

`SDL2`의 키 상태를 기반으로 `누름(Press)`, `유지(Hold)`, `해제(Release)` 상태를 감지하며,
엔진 루프(`Engine::mainLoop`) 내에서 자동으로 갱신된다.

이 모듈은 모든 키를 `RexKey` 열거형으로 통합하여
플랫폼이나 SDL 스캔 코드에 의존하지 않고 직관적인 코드 작성이 가능하도록 설계되었다.

---

## Input Class

## 개요

`Input` 클래스는 SDL 이벤트를 처리하고, 키보드 입력 상태를 실시간으로 추적한다.

각 프레임마다 `update()`를 호출하여 입력상태를 쵯니으로 유지하며,

`isKeyHeld()`, `isKeyPressed()`, `isKeyReleased()`를 통해 다양한 키 이벤트를 감지할 수 있다.

---

## 클래스 정의

```cpp
#pragma once
#include <SDL2/SDL.h>
#include "InputKeys.h"
#include <unordered_map>

class Input {
public:
    Input();

    void update(bool& isRunning);
    bool isKeyHeld(RexKey key);
    bool isKeyPressed(RexKey key);
    bool isKeyReleased(RexKey key);

private:
    const Uint8* state = nullptr;
    std::unordered_map<RexKey, SDL_Scancode> keyMap;
    std::unordered_map<RexKey, bool> prevState;

    void setupKeyMap();
};
```

---

## 멤버 설명

### Input()
생성자.

`setupKeyMap()`을 호출하여 `RexKey`와 SDL 스캔코드를 매핑한다.
이 매핑은 엔진 전반에서 공통적으로 사용된다.

---

### void update(bool& isRunning)

입력 이벤트를 처리하고, 각 키의 현재 상태를 갱신한다.

| 매개변수        | 설명                                         |
| ----------- | ------------------------------------------ |
| `isRunning` | `false`로 설정되면 게임 루프 종료 (ESC 또는 창 닫기 이벤트 시) |

주요 기능:
- SDL 이벤트(`SDL_QUIT`, `SDL_KEYDOWN`) 감지
- 현재 프레임의 키보드 상태(`SDL_GetKeyborardState`) 갱신
- 이전 프레임의 입력 상태(`prevState`) 저장
