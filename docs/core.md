# Core 모듈

Core 모듈은 엔진 실행 루프와 초기화, 종료를 담당한다.

## 클래스: Engine

### init
```cpp
bool init(const char* title, int width, int height);
```

### run

```cpp
void run();
```

엔진 메인 루프를 실행한다. 입력 → 업데이트 → 렌더링 순서를 반복한다.

### quit

```cpp
void quit();
```

엔진을 종료하고 자원을 해제한다.