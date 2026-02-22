# Rex 엔진 개요 (내부/외부 개발자 공용)

이 문서는 다음 두 그룹을 동시에 대상으로 한다.
- 외부 개발자: Rex 기반 런타임/툴 제작
- 내부 개발자: 엔진 아키텍처/시스템 확장

## 1. 현재 엔진 상태
- 언어/빌드: C++20 + CMake + `python3 x.py`
- 렌더링: 모듈형 Deferred 파이프라인(PBR, CSM 그림자, HDR, SSAO, Bloom, ACES 톤매핑)
- 물리: Rust 강체 코어 + C++ 브리지(`PhysicsSystem`)
- UI/에디터: SDL + RexUI(`rex-editor`), Qt 제거 완료
- 에디터 아키텍처: `Engine/Editor/`에 AAA급 통합 제작 환경 골격 추가

## 2. 저장소 구조
```text
Engine/
  Core/        # ECS, 수학, 로거, 윈도우
  Graphics/    # 모듈형 렌더러 스택
  Physics/     # Rust 물리 코어용 C++ 브리지
  Rust/        # Rust 크레이트(물리 코어)
  UI/          # Legacy RexUI + 차세대 프레임워크 스켈레톤
  Editor/      # 통합 에디터 플랫폼 모듈(core/panels/tools/asset/plugin)
  EditorRex/   # 에디터 엔트리
  Runtime/     # 런타임 샌드박스 엔트리
docs/
  english/
  korean/
```

## 3. 빌드/실행
### 3.1 권장 빌드
```bash
python3 x.py
```

### 3.2 추가 빌드 명령
```bash
python3 x.py configure
python3 x.py build
python3 x.py run editor
python3 x.py run editor-legacy
python3 x.py run runtime
```

### 3.3 직접 실행
```bash
./build/rex-editor
./build/rex-editor-legacy
./build/rex-runtime
```

## 4. 런타임 엔트리 권장
기본 통합 예제로 `Engine/Runtime/runtime_main.cpp`를 사용한다.

해당 파일은 다음을 포함한다.
- ECS 초기화
- Rust 물리 루프 연동
- Deferred 렌더러 연동
- 다광원 스트레스 씬
- 후처리 실시간 제어

## 5. 런타임 조작 요약
- 카메라: `WASD + QE`, `Shift`, `RMB + 마우스`
- 물리 상호작용: `LMB`, `Space`, `B`, `J`, `R`, `T`, `G`, `P`, `Delete`
- 그래픽 튜닝: `F1`, `F2`, `F3/F4`, `F5/F6`, `L`, `K`

## 6. 내부 개발 규칙
- Core/Graphics/Physics/UI 경계 준수
- 하위 모듈에서 상위 모듈 의존성 역참조 금지
- 코드 변경 시 문서 동기화
- 머지 전 빌드 검증 필수(`python3 x.py build`)

## 7. 다음 문서
- 렌더링 상세: `docs/korean/graphics.md`
- 물리 연동: `docs/korean/physics.md`
- ECS/컴포넌트: `docs/korean/core.md`, `docs/korean/entity.md`
- 런타임 월드 흐름: `docs/korean/world.md`
- 중장기 계획: `docs/korean/roadmap.md`
- AAA 에디터 설계서: `docs/korean/editor_integrated_production_environment.md`
