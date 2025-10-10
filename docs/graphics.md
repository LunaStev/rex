# Graphics Moudle

`Graphics` 모듈은 SDL2 렌더링 기능을 단순화한 그래픽 관리 클래스다.

색상 지정, 화면 초기화, 사각현 렌더링, 화면 업데이트 등의 기본적인 2D 그래픽 기능을 제공한다.

이 클래스는 `Engine` 내부에서 자동으로 관리되며, 사용자는 `Game::render()` 내에서 이를 사용해 직접 그리기를 수행한다.

## Graphics Class

## 개요

`Graphics` 클래스는 SDL 렌더러(`SDL_Renderer`)를 래핑하여 2D 그래픽 작업을 간단히 수행할 수 있도록 설계된 도우미 클래스다.

주요 기능:
- 배경 색상 설정 및 화면 초기화 (`clear`)
- 기본 도형(사각형) 렌더링 (`drawRect`)
- 프레임 버퍼 출력 (`present`)

---

## 클래스 정의

```cpp
#pragma once
#include <SDL2/SDL.h>

class Graphics {
public:
    Graphics() : renderer(nullptr) {}

    void setRenderer(SDL_Renderer* r);
    void clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void present();

private:
    SDL_Renderer* renderer;
};
```

---

## 멤버 설명

### Graphics()

기본 생성자.

내부 `renderer`를 `nullptr`로 초기화한다.
`Engine` 초기화 시점에서 `setRenderer()`를 통해 실제 SDL 렌더러가 설정된다.

---

### void setRenderer(SDL_Renderer r)*

렌더러 포인터를 설정한다.
이 함수는 `Engine` 내부에서 자동으로 호출되므로 일반 사용자는 호출할 필요가 없다.

| 매개변수 | 설명          |
| ---- | ----------- |
| `r`  | SDL 렌더러 포인터 |


---

### void clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a)

지정한 색상으로 화면을 초기화한다.
보통 각 프레임의 렌더링 시작 시 호출된다.

| 매개변수               | 설명                     |
| ------------------ | ---------------------- |
| `r`, `g`, `b`, `a` | 색상의 RGBA 구성 요소 (0~255) |

예시:

```cpp
graphics.clear(0, 0, 0, 255);  // 검은색으로 화면 초기화
```

---

### void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)

사각형을 그린다.

좌표, 크기, 색상을 지정할 수 있으며, 내부적으로 `SDL_RenderFillRect`를 사용한다.

| 매개변수               | 설명                   |
| ------------------ | -------------------- |
| `x`, `y`           | 사각형의 시작 좌표           |
| `w`, `h`           | 사각형의 너비와 높이          |
| `r`, `g`, `b`, `a` | 사각형 색상 (RGBA, 0~255) |

예시:

```cpp
graphics.drawRect(100, 100, 50, 50, 255, 0, 0, 255); // 빨간 사각형
```

---

### void present()

렌더링된 내용을 화면에 표시한다.
프레임 루프의 마지막 단계에서 호출되어 버퍼를 전환한다.

예시: 

```cpp
graphics.present();  // 버퍼 스왑
```

---

## 예시 코드

```cpp
#include "Graphics/Graphics.h"

void ExampleRender(Graphics& g) {
    g.clear(30, 30, 30, 255);               // 배경 초기화 (회색)
    g.drawRect(100, 100, 200, 100, 0, 255, 0, 255); // 녹색 직사각형
    g.present();                            // 화면 출력
}
```

---

## 의존 관계

| 의존 모듈  | 설명                              |
| ------ | ------------------------------- |
| SDL2   | 렌더링 및 그래픽 기능 제공                 |
| Engine | 렌더러 초기화 및 `setRenderer()` 호출 담당 |

---

## 요약

| 함수                                 | 설명               |
| ---------------------------------- | ---------------- |
| `setRenderer(SDL_Renderer* r)`     | 내부 SDL 렌더러 설정    |
| `clear(r, g, b, a)`                | 배경색으로 화면 초기화     |
| `drawRect(x, y, w, h, r, g, b, a)` | 사각형 렌더링          |
| `present()`                        | 렌더링된 프레임을 화면에 표시 |
