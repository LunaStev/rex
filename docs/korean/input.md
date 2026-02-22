# Input 처리 가이드

Rex의 현재 입력 처리 중심은 SDL 이벤트 루프다. 에디터와 런타임에서 입력 처리 책임을 명시적으로 관리한다.

## 1. 현재 구조
- 이벤트 수집: SDL (`SDL_PollEvent`)
- UI 입력 전달: `UIContext::dispatch*`
- 카메라 입력: 에디터 루프에서 직접 처리

## 2. 에디터 기본 키맵
- 카메라 이동: `W/A/S/D`
- 수직 이동: `Space`/`Ctrl`
- 룩 모드: RMB 누른 상태
- 기즈모 모드: `Q/W/E/R`
- 삭제: `Delete`

## 3. UI와 뷰포트 입력 충돌 해결
우선순위:
1. 포커스된 텍스트 입력
2. UI 캡처 이벤트
3. 뷰포트 입력
4. 전역 단축키

실무 팁:
- `ui.dispatchKeyDown()` 반환값으로 소비 여부 판단
- 텍스트 입력(`SDL_TEXTINPUT`)은 키 이벤트와 분리

## 4. 외부 개발자 적용 패턴
```cpp
while (SDL_PollEvent(&ev)) {
    // 1) UI에게 먼저 전달
    // 2) 소비되지 않으면 카메라/게임 로직 처리
}
```

## 5. 내부 개발자 계획
차세대 RexUI 프레임워크에서는 아래 모듈로 통합:
- `Framework/Input/InputEvent.h`
- `Framework/Input/InputMap.h`
- `Framework/Input/InputRouter.h`

목표:
- 물리 입력과 논리 액션 분리
- 컨텍스트 기반 입력 라우팅
- 키/마우스/게임패드 통합

## 6. 테스트 전략
- 포커스 위젯에서 단축키 오염 여부
- 마우스 캡처 on/off 경계
- 고속 키입력(반복) 안정성
