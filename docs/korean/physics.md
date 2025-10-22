# Physics Module

`Physics` 모듀은 엔티티(Entity)의 움직임과 중력, 충돌을 처리하는 핵심 물리 시스템이다.

`Entity`와 `World` 간의 상호작용을 담당하며,
중력 적용, 지면 충돌처리, 객체 간 충돌 판정(AABB) 기능을 제공한다.

---

## 개요

`Physics` 클래스는 게임 내 물리 연산을 관리한다.
기본적으로 중력, 낙하, 지면 충돌, 엔티티 간 충돌(AABB)을 처리한다.
엔진 루프에서 매 프레임 `apply()`를 호출하여 물리 상태를 갱신한다.

---

## 클래스 정의

```cpp
#pragma once
#include "../Entity/Entity.h"
#include "../World/World.h"

class Physics {
public:
    float gravity = 900.0f;

    void apply(Entity& entity, float dt, World& world);
    bool checkGroundCollision(Entity& entity, World& world);
    static bool isColliding(const Entity& a, const Entity& b);
};
```

---

## 멤버 설명

### float gravity
엔티티에 적용되는 중력의 세기.

기본값은 `900.0f`이며, `dt`(프레임 간 경과 시간)를 곱해 매 프레임마다 하강 속도를 증가시킨다.

| 단위    | 의미     |
| ----- | ------ |
| 픽셀/초² | 중력 가속도 |

예시:

```cpp
physics.gravity = 1200.0f; // 더 강한 중력
```

---

### void apply(Entity& entity, float dt, World& world)
엔티티에 중력을 적용하고, 위치를 업데이트하며, 지면 충돌을 감지/보정한다.

| 매개변수     | 설명              |
| -------- | --------------- |
| `entity` | 물리 연산 대상 엔티티    |
| `dt`     | 프레임 간 경과 시간     |
| `world`  | 충돌 대상이 되는 월드 객체 |

처리 순서:
1. 중력 적용 -> `vy += gravity * dt`
2. 위치 갱신 -> `x += vx * dt`, `y += vy * dt`
3. 지면 충돌 확인 (`checkGroundCollision` 호출)
4. 충돌 시 수직 속도(`vy`) 0으로 설정

기본 구현:

```cpp
entity.setVelocity(entity.getVX(), entity.getVY() + gravity * dt);
entity.setPosition(
    entity.getX() + entity.getVX() * dt,
    entity.getY() + entity.getVY() * dt
);

if (checkGroundCollision(entity, world))
    entity.setVY(0);
```

---

### bool checkGroundCollision(Entity& entity, World& world)
엔티티가 월드의 지면과 충돌했는지 확인한다.
충돌 시, 엔티티의 위치를 보정해 지면 위로 이동시킨다.

| 매개변수     | 설명       |
| -------- | -------- |
| `entity` | 충돌 감지 대상 |
| `world`  | 지형 정보    |

반환값:

* 충돌 발생 시 `true`
* 충돌 없을 경우 `false`