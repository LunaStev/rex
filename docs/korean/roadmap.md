# Rex 엔진 로드맵 (내부/외부 개발자 공용)

## 1. 최근 완료
### 렌더링
- Deferred 파이프라인 전환
- RenderGraph 스타일 패스 시퀀싱
- PBR Cook-Torrance 셰이딩 경로
- Directional CSM Atlas 그림자 경로
- HDR + SSAO + Bloom + ACES 톤매핑 체인
- 런타임 스트레스 씬 통합(`Engine/Runtime/runtime_main.cpp`)

### 물리
- Rust 물리 코어와 C++ 브리지 연동 완료
- 쿼터니언 동역학/제약 흐름 런타임 적용

### 에디터/UI
- Qt 제거 완료
- SDL + RexUI 에디터 경로 안정화
- 차세대 산업용 RexUI 설계 스켈레톤(아키텍처 우선) 고정

## 2. 단기 계획 (그래픽)
### 목표
Deferred 전환 이후 품질/성능 격차 보완

### 작업
- Forward+ tile/cluster light culling
- RenderGraph 리소스 aliasing/reuse 강화
- cascade 안정화(텍셀 스냅/스플릿 튜닝)
- soft shadow 품질 단계 옵션화

### 완료 기준
- 대량 광원 환경에서 안정적인 프레임 타임
- 카메라 이동 시 그림자 떨림 감소
- 기존 Deferred 품질 대비 회귀 없음

## 3. 중기 계획 (그래픽 + 런타임)
### 목표
환경 기반 조명 파이프라인 완성 및 시네마틱 품질 향상

### 작업
- IBL 전체 경로(irradiance + prefiltered env + BRDF LUT)
- color grading LUT 파이프라인
- 고급 효과(TAA/DOF/motion blur) 선택 구현 + 품질 게이트
- GPU 프로파일 마커 및 패스별 계측 도구

### 완료 기준
- 환경광 기반 PBR 응답 일관성 확보
- 패스별 GPU 비용 계측/문서화
- 런타임 품질 프리셋(low/mid/high) 제공

## 4. UI 프레임워크 트랙
### 목표
설계 스켈레톤에서 실사용 프레임워크 모듈로 이행

### 작업
- StateStore/Binding/Diff 핵심 루프
- DockManager + Undo/Redo 연동
- 대규모 Hierarchy virtualization

### 완료 기준
- 대규모 씬에서도 안정적인 에디터 상호작용
- undo/redo 시 UI 상태 전이 일관성 확보

## 5. 내부 품질 목표
- 주요 시스템 변경 시 빌드/실행 검증 유지
- 구현과 문서 동기화 유지
- 성능 민감 경로는 항상 계측 가능 상태 유지
