# Rex 엔진 로드맵 (내부/외부 개발자 공용)

## 1. 현재 완료 상태
### 엔진
- ECS 기본 구조
- OpenGL 렌더러
- 물리 고도화(Quat, CCD, Dynamic AABB Tree, 2~4점 매니폴드)

### 에디터
- Qt 제거
- SDL + RexUI 기반 에디터 전환

### UI 프레임워크
- 산업용 RexUI 스켈레톤(Design Freeze + Interface-first) 생성

## 2. 단기 계획 (Phase A/B)
### 목표
- 차세대 RexUI 최소 동작 루프 확보

### 작업
- Core Geometry/Widget interface 구현
- WidgetTree MVP
- LayoutEngine/EventRouter 기본 버전

### 완료 기준
- 기본 위젯 트리 렌더 + 입력 이벤트 전달 동작
- 최소 단위 테스트 통과

## 3. 중기 계획 (Phase C/D)
### 목표
- 상태 중심 UI 파이프라인 정착

### 작업
- UIStateStore + Binding 최소 구현
- DiffEngine + DrawCommandBuilder
- OpenGL backend 정식 구현(`IRenderBackend`)

### 완료 기준
- 상태 변경 -> 최소 패치 -> 렌더 반영
- 성능 계측 지표 노출

## 4. 장기 계획 (Phase E+)
### 목표
- 산업용 에디터 기능 완성

### 작업
- DockManager
- Undo/Redo 완전 연동
- Virtualized Hierarchy
- Vulkan backend
- Plugin SDK

### 완료 기준
- 대규모 씬에서도 편집 안정성 확보
- 백엔드 교체 시 상위 레이어 코드 변경 최소

## 5. 외부 개발자 영향
- 단기: API 변화 적음, 문서 보강
- 중기: UI/입력 API 일부 표준화
- 장기: 확장 가능한 플러그인/툴체인 인터페이스 제공

## 6. 내부 품질 목표
- 빌드/테스트 자동화
- 회귀 체크리스트 운영
- 장시간 실행 안정성(메모리/리소스 누수) 확보
