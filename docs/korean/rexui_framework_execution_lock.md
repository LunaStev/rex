# RexUI 실행 잠금 문서 (Design Freeze + Incremental Delivery)

이 문서는 "한 번에 완성"이 아닌 "구조 선확정 후 단계 구현"을 강제하기 위한 운영 문서다.

## 1. 적용 대상
- 내부 엔진 개발자
- 에디터/툴체인 UI 기여자
- 아키텍처/성능 리뷰어

## 2. 고정된 원칙
1. 아키텍처를 먼저 확정한다.
2. 인터페이스/폴더/파일을 먼저 만든다.
3. TODO 메타데이터를 먼저 잠근다.
4. 모듈 단위로 단계 구현한다.

## 3. 현재 잠금 상태
- 스켈레톤 경로: `Engine/UI/RexUI/`
- 파일 수: 54
- TODO 수: 54
- 각 TODO는 다음 메타데이터를 포함한다.
  - 고유 ID
  - 책임
  - 요구사항
  - 의존성
  - 구현 단계
  - 성능 고려사항
  - 테스트 전략

## 4. 구현 단계
- Phase A: Geometry, Widget 인터페이스, WidgetTree MVP
- Phase B: LayoutEngine, EventRouter MVP
- Phase C: StateStore, Binding 최소 구현
- Phase D: DiffEngine, DrawCommand/RenderGraph
- Phase E: DockManager, Undo/Redo, Virtualization

## 5. 변경 통제 규칙
- 구현 중 레이어 경계 임의 변경 금지
- 하위 모듈에서 상위 모듈 참조 금지
- UI 직접 모델 수정 금지(반드시 CommandBus)
- Renderer 종속 코드가 Framework/Runtime로 올라오지 않게 유지

## 6. 코드 리뷰 체크리스트
- TODO ID와 구현 범위가 일치하는가
- 의존성 방향 위반이 없는가
- 테스트가 TODO의 테스트 전략을 충족하는가
- 성능 민감 경로 계측 지표가 있는가

## 7. 외부 개발자 커뮤니케이션
- 주요 API 변경 시 `overview.md`와 모듈 문서를 동시에 업데이트
- 브레이킹 변경은 migration note를 함께 제공
