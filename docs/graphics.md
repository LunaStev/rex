# Graphics 모듈

Graphics 모듈은 2D 렌더링을 담당한다.

## 클래스: Graphics

### clear

```cpp
void clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
```

배경을 지정된 색으로 초기화한다.

### drawRect

```cpp
void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
```

사각형을 그린다.

### present

```cpp
void present();
```

렌더링된 내용을 화면에 출력한다.

