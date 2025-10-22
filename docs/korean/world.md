# World Module

`World` 모듈은 게임의 지형(타일 기반 맵)을 관리하는 시스템이다.

`Tile` 구조체를 기반으로 땅, 잔디, 빈 공간 등의 지형을 표현하며,

지형 생성(`generateFlat`)과 렌더링(`redner`), 충돌/지면 계산(`getGroundY`) 기능을 제공한다.

`Entity`는 `World`를 참고하여 지형과의 상호작용(충돌, 착지 등)을 수행할 수 있다.

---

## 개요

`World` 클래스는 2D 타일 맵을 관리한다.

각 타일은 `Tile` 구조체로 정의되며, `type` 값으로 종류를 구분한다.

기본적으로 `generateFlat()`을 통해 평평한 땅을 생성하고,
`render()`를 통해 화면에 타일을 그리며,
`getGroundY()`를 통해 특정 위치의 지면 높이를 계산할 수 있다.

---

## 클래스 정의 

```cpp
#pragma once
#include <vector>
#include "../Graphics/Graphics.h"

struct Tile {
    int type; // 0=empty, 1=ground, 2=grass
};

class World {
public:
    static const int TILE_SIZE = 40;
    std::vector<Tile> tiles;
    int width, height;

    void generateFlat(int w, int h, int groundY);
    void render(Graphics& g);
    float getGroundY(float x, float yBottom);
};
```

---

## 멤버 설명

### static const int TILE_SIZE
한 타일의 크기(픽셀 단위).

기본값은 `40`이며, 월드 내 좌표 계산 및 렌더링 시 기준 단위로 사용된다.

---

### std::vector<Tile> tiles
맵의 모든 타일데이터를 1차원 벡터로 저장한다.

좌표 접근 시 `tile[y * width + x]` 형태로 인덱싱한다.

---

### int width, height
타일 맵의 가로(`width`), 세로(`height`) 크기.

---

### void generateFlat(int w, int h, int groundY)
단순한 평명 지형을 생성한다.

지정된 높이(`groundY`)를 기준으로 그 아래는 흙(`ground`), 해당 줄은 잔디(`grass`), 그 위는 빈 공간(`empty`)으로 채운다.

| 매개변수      | 설명               |
| --------- | ---------------- |
| `w`       | 맵의 가로 타일 개수      |
| `h`       | 맵의 세로 타일 개수      |
| `groundY` | 잔디가 생성될 Y 타일 인덱스 |

기본 동작:

```cpp
if (y > groundY) t.type = 1;
else if (y == groundY) t.type = 2;
else t.type = 0;
```

예시:

```cpp
world.generateFlat(32, 18, 10); // 32x18 맵, y=10에서 땅 생성
```

---

### void render(Graphics& g)
월드의 모든 타일을 화면에 그린다.

| 매개변수 | 설명                     |
| ---- | ---------------------- |
| `g`  | `Graphics` 객체 (렌더링 담당) |

기본 색상 팔레트:

| 타일 타입 | 색상                | 의미   |
| ----- | ----------------- | ---- |
| 0     | 없음                | 빈 공간 |
| 1     | 갈색 (90, 60, 30)   | 흙    |
| 2     | 초록색 (30, 200, 60) | 잔디   |

예시:

```cpp
world.render(graphics);
```

---

### float getGorundY(float x, flaot yBottom)
주어진 X 좌표 아래쪽에 존재하는 가장 가까운 지면의 Y 위치를 반환한다.

충돌 처리나 착지 판정 등에 사용된다.

| 매개변수      | 설명               |
| --------- | ---------------- |
| `x`       | 검색할 X 좌표         |
| `yBottom` | 엔티티의 바닥 위치 (참조용) |

반환값:

지면 타일 상단 Y 좌표(픽셀)

지면이 없을 경우, 월드 맨 아래(`height * TILE_SIZE`) 반환.

예시:

```cpp
float groundY = world.getGroundY(player.getX(), player.getY() + player.getHeight());
```

---

## Tile Structure

```cpp
struct Tile {
    int type; // 0=empty, 1=ground, 2=grass
};
```

| 값   | 의미            |
| --- | ------------- |
| `0` | 빈 공간 (그리지 않음) |
| `1` | 흙 (Ground)    |
| `2` | 잔디 (Grass)    |

---

## 예시 코드

```cpp
#define SDL_MAIN_HANDLED
#include "Core/Engine.h"
#include "World/World.h"

class WorldTest : public Game {
    World world;

public:
    WorldTest() {
        world.generateFlat(20, 15, 10);
    }

    void update(float dt, Input& input) override {
        // world is fixed, no update
    }

    void render(Graphics& g) override {
        g.clear(50, 100, 200, 255);
        world.render(g);
        g.present();
    }
};

int main() {
    Engine::run<WorldTest>("World Test", 800, 600);
}
```

---

## 의존 관계

| 의존 모듈      | 설명                   |
| ---------- | -------------------- |
| `Graphics` | 타일 렌더링 담당            |
| `Entity`   | 지형과의 상호작용 (충돌, 착지 등) |

---

## 요약 

| 함수                            | 설명              |
| ----------------------------- | --------------- |
| `generateFlat(w, h, groundY)` | 평면 지형 생성        |
| `render(graphics)`            | 타일 맵 렌더링        |
| `getGroundY(x, yBottom)`      | 지정 위치의 지면 높이 계산 |
