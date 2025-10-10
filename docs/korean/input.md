# Input 모듈

Input 모듈은 키보드 입력을 관리한다.

## 클래스: Input

### update

```cpp
void update(bool& isRunning);
```

현재 프레임의 입력 상태를 갱신한다.
isRunning을 false로 설정하면 엔진 루프가 종료된다.

### isKeyHeld

```cpp
bool isKeyHeld(SDL_Scancode key);
```

특정 키가 현재 눌려 있는지 확인한다.
예:

```cpp
if (input.isKeyHeld(SDL_SCANCODE_RIGHT)) { ... }
```