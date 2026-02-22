# RexUI 산업용 아키텍처 가이드

이 문서는 RexUI를 단순 위젯 라이브러리에서 산업용 UI 프레임워크로 확장하기 위한 설계 기준을 설명한다.

## 1. 왜 필요한가
- 에디터가 커질수록 도킹, 상태관리, Undo/Redo, 대규모 리스트 성능이 핵심 병목이 된다.
- 기존 즉시형 UI만으로는 유지보수/성능/확장성 한계가 크다.

## 2. 목표
- 기능/구조 패리티: 기존 Qt 에디터 수준 동작 재현
- 아키텍처 확장성: 플러그인/커스텀 위젯/백엔드 교체
- 성능: diff 업데이트 + virtualization + layout cache

## 3. 레이어 구조
- Core: Widget/Event/Style/Geometry 계약
- Framework: State/Binding/Commands/Input/Docking/Animation/Plugins
- Runtime: WidgetTree/Diff/Layout/RenderGraph
- Renderer: IRenderBackend + OpenGL/Vulkan 구현
- Widgets: Basic/Editor 위젯
- Integration: Editor 도메인 브리지

## 4. 내부 개발자 관점 설계 규칙
- Interface-first
- 단방향 의존성
- UI에서 직접 모델 수정 금지(CommandBus)
- Renderer 종속 코드 상위 침투 금지

## 5. 외부 개발자 관점 사용 모델
- 장기적으로는 선언형 View + 상태 바인딩 모델 제공
- 커스텀 위젯은 Registry를 통해 등록
- 플러그인 방식으로 새 패널/툴 주입 가능

## 6. 핵심 기능 축
- Routed Event (capture/bubble)
- StateStore + BindingEngine
- CommandBus + Undo/Redo
- DockManager
- Timeline transition
- Virtualized list/tree

## 7. 성능 원칙
- 전체 재빌드 대신 subtree diff
- 보이는 영역만 realize
- layout/draw 결과 캐시
- draw command 배치 최적화

## 8. 참고
- 실행 잠금 문서: `rexui_framework_execution_lock.md`
- 실제 스켈레톤 코드: `Engine/UI/RexUI/`
