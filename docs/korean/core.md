# Core Moudle 

## Engine Class

## 개요

`Engine` 클래스는 게임 실행의 핵심 루프를 관리하는 프레임워크의 중심 클래스다.

SDL을 기반으로 윈도우 생성, 렌더러 초기화, 입력 처리, 그리고 메인 루프 실행을 담당한다.

게임 개발자는 `Game` 클래스를 상속 받은 자신의 게임 클래스를 정의하고, `Engine::run<YourGame>()`을 호출하여 손쉽게 게임을 실행할 수 있다.

---

## 클래스 정의

```cpp
class Engine {
public:
    Engine();
    ~Engine();

    bool init(const char* title, int width, int height);

    template<typename T>
    static void run(const char* title = "My 2D Engine", int width = 800, int height = 600);

    void quit();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    Graphics graphics;
    Input input;

    void mainLoop(Game& game);
};
```

---

## 멤버 설명

### 생성자 & 소멸자
- `Engine()`
    - SDL 관련 객체(`window`, `renderer`)를 초기화하지 않은 상태로 생성한다.
- `~Engine()`
    - `quit()`을 자동으로 호출하여 SDL 자원을 정리한다.

---

### bool init(const char title, int width, int height)*

게임 엔진을 초기화한다.

SDL 서브시스템, 윈도우, 렌더러를 생성하며, 내부적으로 `Graphics`와 `Input` 객체를 초기화한다.

| 매개변수     | 설명     |
| -------- | ------ |
| `title`  | 윈도우 제목 |
| `width`  | 윈도우 너비 |
| `height` | 윈도우 높이 |

### 반환값:

초기화 성공 시 `true`, 실패 시 `false`.

### 예외처리:

초기화 실패 시 `std::cerr`를 통해 SDL 에러 메시지를 출력한다.

---

### template<typename T> static void run(const char title, int width, int height)*

사용자 정의 `Game` 클래스를 받아 엔진 루프를 실행한다.

| 템플릿 매개변수 | 설명                          |
| -------- | --------------------------- |
| `T`      | `Game` 클래스를 상속받은 사용자 정의 클래스 |


설명:

- 내부적으로 `Engine` 인스턴스를 생성하고 `init()` -> `mainLoop()`를 순서대로 호출한다.
- `T`는 반드시 `Game`을 상속받고, `update()` 및 `render()`를 구현해야 한다.

사용 예시:

```cpp
class MyGame : public Game {
public:
    void update(float dt, Input& input) override {
        // 게임 로직
    }

    void render(Graphics& g) override {
        // 그리기
    }
};

int main() {
    Engine::run<MyGame>("My First Game", )
}
```

---

### void quit()

SDL 윈도우 및 렌더러를 해제하고, SDL 서브시스템을 종료한다.

엔진이 종료될 때 자동으로 호출된다.

---

### void mainLoop(Game& game) (private)

게임 메인 루프를 실행한다.

루프 내부에서는 다음 과정을 반복한다:

1. 입력 갱신 (`input.update(isRunning)`)
2. 델타타임 계산
3. `game.update(dt,input)` 호출
4. `graphics.clear()` -> `game.render(graphics)` -> `graphics.present()`
5. 프레임 제한 (`SDL_Delay(16)`)

---

## 내부 구조 요약

| 구성요소       | 역할               |
| ---------- | ---------------- |
| `Graphics` | SDL 렌더링 담당       |
| `Input`    | 키보드 입력 처리        |
| `Game`     | 사용자 정의 게임 로직 클래스 |

---

## 의존 관계

- `Graphics` - 그래피 출력 (렌더러 설정 및 관리)
- `Input` - 입력 이벤트 처리
- `Game` - 게임 로직 추상화 클래스

---

## Game Class

## 개요

`Game` 클래스는 사용자가 직접 상속하여 자신의 게임 로직을 구현하는 추상 클래스이다.
`update()`와 `render()` 두 메서드를 반드시 구현해야 한다.

## 클래스 정의

```cpp
#pragma once
#include "../Graphics/Graphics.h"

class Game {
public:
    virtual ~Game() = default;

    virtual void update(float dt, Input& input) = 0;
    virtual void render(Graphics& g) = 0;
};
```

---

## 멤버 설명

### virtual void update(float dt, Input& input) = 0

매 프레임마다 호출되어 게임 로직을 갱신한다.

| 매개변수    | 설명                |
| ------- | ----------------- |
| `dt`    | 프레임 간 경과 시간(초 단위) |
| `input` | 입력 처리 객체          |

--- 

### virtual void render(Graphics& g) = 0

렌더링을 수행한다.
게임 내 모든 그리기 로직은 이 메서드에서 실행된다.

| 매개변수 | 설명                     |
| ---- | ---------------------- |
| `g`  | `Graphics` 객체 (렌더링 도구) |

---

## 설계 철학

- `update()`와 `render()`를 명확히 분리하여 프레임 단위의 논리와 시각 출력을 구분

- `Engine`과의 결합을 최소화하여 테스트와 유지보수 용이성 확보

---

## 예시

```cpp
#include "Core/Engine.h"

class ExampleGame : public Game {
    float x = 0.0f;

public:
    void update(float dt, Input& input) override {
        x += 100 * dt;
    }

    void render(Graphics& g) override {
        g.drawRect((int)x, 100, 50, 50, 255, 0, 0, 255);
    }
};

int main() {
    Engine::run<ExampleGame>("Example Game", 800, 600);
    return 0;
}
```

---

## 요약

| 클래스      | 역할                        |
| -------- | ------------------------- |
| `Engine` | 게임 루프, 렌더러, 입력 시스템을 관리    |
| `Game`   | 사용자 정의 게임 로직을 정의하는 추상 클래스 |
