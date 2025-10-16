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

---

### bool isKeyHeld(RexKey key)

특정 키가 현재 눌려있는 상태인지 확인한다.

| 매개변수  | 설명                   |
| ----- | -------------------- |
| `key` | 확인할 키 (`RexKey` 열거형) |

반환값:

눌려있으면 `true`, 아니면 `false`.

예시: 

```cpp
if (input.isKeyHeld(RexKey::RIGHT)) {
    box.setPosition(box.getX() + speed, box.getY());
}
```

---

### bool isKeyPressed(RexKey key)

특정 키가 이번 프레임에 처음 눌렸는지 확인한다.

| 매개변수  | 설명                   |
| ----- | -------------------- |
| `key` | 확인할 키 (`RexKey` 열거형) |

반환값:

이번 프레임에서 새로 눌렸다면 `true`, 아니면 `false`.

예시:

```cpp
if (input.isKeyPressed(RexKey::SPACE)) {
    player.jump();
}
```

---

### bool isKeyReleased(RexKey key)

특정 키가 이번 프레임에 해제되었는지 확인한다.

| 매개변수  | 설명                   |
| ----- | -------------------- |
| `key` | 확인할 키 (`RexKey` 열거형) |

반환값:

이번 프레임에서 해제되었다면 `true`, 아니면 `false`.

예시:

```cpp
if (input.isKeyReleased(RexKey::SHIFT)) {
    player.stopSprinting();
}
```

---

### bool isKeyReleased(RexKey key)

특정 키가 이번 프레임에 해제되었는지 확인한다.

| 매개변수  | 설명                   |
| ----- | -------------------- |
| `key` | 확인할 키 (`RexKey` 열거형) |

반환값:

이번 프레임에서 해체되었다면 `true`, 아니면 `false`.

예시:

```cpp
if (input.isKeyReleased(RexKey::SHIFT)) {
    player.stopSprinting();
}
```

---

### void setupKeyMap() (private)

`RexKey`와 SDl 스캔코드를 매핑한다.

`A~Z`, 방향키, SPACE, ESCAPE, RETURN, SHIFT, CTRL, ALT 키를 지원한다.

| RexKey   | SDL Scancode          |
| -------- | --------------------- |
| `A`      | `SDL_SCANCODE_A`      |
| `B`      | `SDL_SCANCODE_B`      |
| `C`      | `SDL_SCANCODE_C`      |
| ...      | ...                   |
| `LEFT`   | `SDL_SCANCODE_LEFT`   |
| `RIGHT`  | `SDL_SCANCODE_RIGHT`  |
| `SPACE`  | `SDL_SCANCODE_SPACE`  |
| `ESCAPE` | `SDL_SCANCODE_ESCAPE` |
| `RETURN` | `SDL_SCANCODE_RETURN` |
| `SHIFT`  | `SDL_SCANCODE_LSHIFT` |
| `CTRL`   | `SDL_SCANCODE_LCTRL`  |
| `ALT`    | `SDL_SCANCODE_LALT`   |

---

## RexKey Enum

## 개요

`RexKey`는 키 입력을 표현하는 내부 공통 열거형이다.

SDL 스캔코드 대신 사용하여, 코드의 가독성과 유지보수성을 높인다.

---

### 정의

```cpp
#pragma once

enum class RexKey {
    UNKNOWN = 0,
    A, B, C, D, E, F, G,
    H, I, J, K, L, M, N,
    O, P, Q, R, S, T, U,
    V, W, X, Y, Z,
    UP, DOWN, LEFT, RIGHT,
    SPACE, ESCAPE,
    RETURN,
    SHIFT, CTRL, ALT,
};
```

---

## 예시 코드

```cpp
void update(float dt, Input& input) override {
    float speed = 300 * dt;
    if (input.isKeyHeld(RexKey::RIGHT)) box.setPosition(box.getX() + speed, box.getY());
    if (input.isKeyHeld(RexKey::LEFT))  box.setPosition(box.getX() - speed, box.getY());
    if (input.isKeyHeld(RexKey::UP))    box.setPosition(box.getX(), box.getY() - speed);
    if (input.isKeyHeld(RexKey::DOWN))  box.setPosition(box.getX(), box.getY() + speed);

    if (input.isKeyHeld(RexKey::R)) box.setColor(255, 0, 0);
    if (input.isKeyHeld(RexKey::G)) box.setColor(0, 255, 0);
    if (input.isKeyHeld(RexKey::B)) box.setColor(0, 0, 255);
}
```

```cpp
#include "Input/Input.h"

void PlayerControl(Input& input) {
    if (input.isKeyHeld(RexKey::W))
        player.moveUp();

    if (input.isKeyPressed(RexKey::SPACE))
        player.jump();

    if (input.isKeyReleased(RexKey::CTRL))
        player.stopCrouching();
}
```

---

## 의존 관계

| 의존 모듈  | 설명                          |
| ------ | --------------------------- |
| SDL2   | 키보드 이벤트 및 스캔코드 제공           |
| Engine | `update()` 호출 및 루프 내 이벤트 관리 |


---

## 요약

| 함수                        | 설명                 |
| ------------------------- | ------------------ |
| `update(bool& isRunning)` | 입력 이벤트 갱신          |
| `isKeyHeld(key)`          | 키가 눌려 있는 상태인지 확인   |
| `isKeyPressed(key)`       | 이번 프레임에 처음 눌렸는지 확인 |
| `isKeyReleased(key)`      | 이번 프레임에 해제되었는지 확인  |
