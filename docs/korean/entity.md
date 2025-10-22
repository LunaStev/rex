# Entity Module

`Entity` 모듈은 프레임워크 내에서 모든 게임 오브젝트의 기본 클래스다.

위치, 속도, 크기, 색상, 생존 상태 등의 공통 속성을 포함하며,

`update()`와 `render()`를 오버라이드하여 개별 엔티티의 행동과 시각적 표현을 정의할 수 있어.

---

## 개요

`Entity` 클래스는 게임 내 모든 물체(플레이어, 적, 탄환, 오브젝트 등)의 기반 클래스다.

위치(`x`, `y`), 속도(`vx`, `vy`), 크기(`width`, `height`), 색상, 생존 상태 등의 속성을 갖고 있으며

`update()`로 물리적 갱신을, `render()`로 시각적 표현을 수행한다.

---

## 클래스 정의

```cpp
#pragma once
#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../World/World.h"

class Entity {
protected:
    float x, y;          // Location
    float vx, vy;        // Speed
    float width, height; // Size
    bool alive;          // Active status
    Uint8 r, g, b, a;    // Color (Temporary)

public:
    Entity(float x=0, float y=0, float w=40, float h=40)
        : x(x), y(y), vx(0), vy(0), width(w), height(h),
          alive(true), r(255), g(255), b(255), a(255) {}

    virtual ~Entity() = default;

    virtual void update(float dt, Input& input, World& world);
    virtual void render(Graphics& g);

    // Location
    inline float getX() const;
    inline float getY() const;
    inline void setPosition(float nx, float ny);

    // Velocity
    inline float getVX() const;
    inline float getVY() const;
    inline void setVX(float nvx);
    inline void setVY(float nvy);
    inline void setVelocity(float nvx, float nvy);

    // Size
    inline float getWidth() const;
    inline float getHeight() const;

    // Color
    inline void setColor(Uint8 nr, Uint8 ng, Uint8 nb, Uint8 na=255);

    // Life state
    inline bool isAlive() const;
    inline void kill();
};
```

---

## 멤버 설명

### Entity(float x=0, float y=0, float w=40, float h=40)

엔티티를 생성한다.

위치, 크기, 속도, 색상, 생존 상태를 초기화한다.

| 매개변수     | 설명          |
| -------- | ----------- |
| `x`, `y` | 초기 위치       |
| `w`, `h` | 엔티티의 너비와 높이 |

---

### void update(float dt, Input& input, World& world)

엔티티의 상태를 갱신한다.

기본 구현에서는 속도(`vx`, `vy`)를 기반으로 좌표(`x`, `y`)를 이동시킨다.

| 매개변수    | 설명                 |
| ------- | ------------------ |
| `dt`    | 프레임 간 경과 시간 (초 단위) |
| `input` | 입력 정보              |
| `world` | 현재 엔티티가 속한 월드      |

오버라이드 가능:

플레이어나 NPC의 이동, 충돌, AI 등 논리를 이 메서드 안에 구현할 수 있다.

기본 구현:

```cpp
x += vx * dt;
x += vy * dt;
```

---

### void render(Graphics& g)

엔티티를 렌더링한다.

기본 구현에서는 단색 사각형(`drawRect`)으로 엔티티를 표시한다.

| 매개변수 | 설명                     |
| ---- | ---------------------- |
| `g`  | `Graphics` 객체 (렌더링 도구) |

기본 구현:

```cpp
graphics.drawRect((int)x, (int)y, (int)width, (int)height, r, g, b, a);
```

---

## 위치 관련 메서드

| 메서드                                    | 설명         |
| -------------------------------------- | ---------- |
| `float getX() const`                   | 현재 X 좌표 반환 |
| `float getY() const`                   | 현재 Y 좌표 반환 |
| `void setPosition(float nx, float ny)` | 새 위치 설정    |

---

## 속도 관련 메서드

| 메서드                                      | 설명           |
| ---------------------------------------- | ------------ |
| `float getVX() const`                    | X축 속도 반환     |
| `float getVY() const`                    | Y축 속도 반환     |
| `void setVX(float nvx)`                  | X축 속도 설정     |
| `void setVY(float nvy)`                  | Y축 속도 설정     |
| `void setVelocity(float nvx, float nvy)` | 두 축 속도 동시 설정 |

---

## 크기 관련 메서드

| 메서드                       | 설명        |
| ------------------------- | --------- |
| `float getWidth() const`  | 엔티티 너비 반환 |
| `float getHeight() const` | 엔티티 높이 반환 |

---

## 색상 관련 메서드

| 메서드                                                     | 설명        |
| ------------------------------------------------------- | --------- |
| `void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a=255)` | 엔티티 색상 설정 |

예시:

```cpp
entity.setColor(255, 0, 0); // 빨간색
```

---

## 생존 상태 관련 메서드

| 메서드                    | 설명              |
| ---------------------- | --------------- |
| `bool isAlive() const` | 엔티티가 활성 상태인지 확인 |
| `void kill()`          | 엔티티를 비활성 상태로 전환 |


---

## 예시 코드

```cpp
#include "Entity/Entity.h"

class Player : public Entity {
public:
    void update(float dt, Input& input, World& world) override {
        if (input.isKeyHeld(RexKey::A)) setVX(-100);
        else if (input.isKeyHeld(RexKey::D)) setVX(100);
        else setVX(0);

        Entity::update(dt, input, world); // 기본 이동 처리
    }

    void render(Graphics& g) override {
        setColor(0, 200, 255); // 하늘색
        Entity::render(g);
    }
};
```

---

## 의존 관계

| 의존 모듈      | 설명                  |
| ---------- | ------------------- |
| `Graphics` | 엔티티 시각화             |
| `Input`    | 사용자 입력 처리           |
| `World`    | 충돌, 맵 경계 등 월드 관련 정보 |

---

## 요약 

| 함수                         | 설명        |
| -------------------------- | --------- |
| `update(dt, input, world)` | 엔티티 상태 갱신 |
| `render(graphics)`         | 엔티티 렌더링   |
| `setPosition(x, y)`        | 위치 설정     |
| `setVelocity(vx, vy)`      | 속도 설정     |
| `setColor(r, g, b, a)`     | 색상 변경     |
| `kill()`                   | 비활성화      |
