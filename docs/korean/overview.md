# Rex 엔진 개발 문서 (내부/외부 공용)

이 문서는 **Rex 엔진을 사용하는 개발자(외부)**와 **Rex 엔진을 개발하는 개발자(내부)**를 동시에 대상으로 한다.

## 1. 문서 대상
- 외부 개발자:
  - 엔진을 이용해 게임/툴을 만들려는 사용자
  - 실행 방법, API 사용 흐름, 문제 해결이 필요함
- 내부 개발자:
  - 엔진 소스 자체를 확장/리팩토링/최적화하는 기여자
  - 아키텍처 경계, 개발 규칙, 품질 기준이 필요함

## 2. 현재 엔진 상태 요약
- 언어/빌드: C++20 + CMake
- 그래픽: OpenGL 렌더링 파이프라인 (`Renderer`)
- 물리: 강체 기반 물리 (`PhysicsWorld`, `PhysicsSystem`)
- UI: Legacy RexUI + 차세대 RexUI 스켈레톤 병행
- 에디터: SDL + RexUI 기반 (`rex-editor`)

## 3. 저장소 구조
```text
Engine/
  Core/        # ECS, 수학, 윈도우/로거
  Graphics/    # 렌더러, 셰이더, 메시/모델
  Physics/     # 물리 월드, 강체, ECS 브리지
  UI/          # Legacy RexUI + New RexUI skeleton
  EditorRex/   # 에디터 실행 진입점
  Runtime/     # 런타임 샘플 실행 진입점
docs/
  korean/      # 한국어 문서
  english/     # 영어 문서
```

## 4. 필수 의존성
- OpenGL
- SDL2
- Freetype
- C++20 컴파일러

Fedora 계열 예시:
```bash
sudo dnf install gcc-c++ cmake SDL2-devel freetype-devel mesa-libGL-devel
```

## 5. 빌드/실행
### 5.1 빌드
```bash
cmake -S . -B build
cmake --build build -j4
```

### 5.2 실행
```bash
./build/rex-editor
./build/rex-runtime
```

## 6. 외부 개발자용 빠른 시작
### 6.1 최소 런타임 루프 구성
1. `Window` 생성
2. `Scene` 생성
3. `MeshRenderer`, `Transform` 컴포넌트 구성
4. `PhysicsSystem.update(scene, dt)` 호출
5. `Renderer.render(scene, camera, view, ...)` 호출

핵심 예시는 `Engine/Runtime/runtime_main.cpp`를 기준으로 시작하는 것을 권장한다.

### 6.2 에디터 사용 포인트
- 메뉴에서 Add/Delete로 엔티티 관리
- Hierarchy에서 선택
- Details에서 Transform 편집 및 Apply
- RMB + WASD로 카메라 이동

## 7. 내부 개발자용 작업 원칙
### 7.1 레이어 경계
- Core는 상위 모듈 정책을 몰라야 한다.
- 렌더러 종속 코드는 Renderer 하위에 둔다.
- UI는 Command/State 기반으로 모델을 간접 변경한다.

### 7.2 문서/코드 동기화
아래 파일을 변경하면 관련 문서도 함께 갱신한다.
- 물리 로직: `docs/korean/physics.md`
- 렌더 파이프라인: `docs/korean/graphics.md`
- UI 아키텍처: `docs/korean/rexui_*.md`

### 7.3 품질 기준
- 빌드 성공
- 기능 회귀 없음
- 문서/코드 불일치 없음
- 성능 민감 경로(물리 step, 렌더 루프, UI diff) 계측 가능

## 8. 문제 해결
### 8.1 실행이 바로 종료됨
- SDL/GL 초기화 로그 확인
- `Window` 생성 실패 로그 확인 (`Engine/Core/Window.cpp`)

### 8.2 폰트가 보이지 않음
- `Engine/UI/Fonts/Roboto-Regular.ttf` 존재 확인
- `RexUIRenderer` 폰트 후보 경로 확인

### 8.3 물리가 흔들림/관통
- solver iteration 증가
- substep 증가
- CCD 활성화 확인

## 9. 관련 문서
- `core.md`, `entity.md`, `graphics.md`, `physics.md`, `input.md`, `world.md`
- `roadmap.md`
- `rexui_slate_grade_architecture.md`
- `rexui_framework_execution_lock.md`
