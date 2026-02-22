# Core 모듈 상세 가이드

Core는 엔진 전반이 의존하는 기반 계층이다.

## 1. 핵심 구성
- `Scene`: ECS 저장소
- `Components`: 공용 데이터 스키마
- `RexMath`: 벡터/행렬/쿼터니언
- `Window`: SDL + OpenGL 컨텍스트
- `Logger`: 포맷 기반 로그

## 2. Scene 설계
### 2.1 데이터 구조
- `Scene`은 `std::unordered_map<std::type_index, std::unique_ptr<ComponentPool>>`를 가진다.
- 컴포넌트 타입별로 `TypedComponentPool<T>`를 분리한다.

### 2.2 사용 계약
- `createEntity()`는 ID만 발급한다.
- 실제 의미는 컴포넌트 조합이 만든다.
- `destroyEntity()`는 모든 풀에서 해당 ID를 지운다.

### 2.3 반복 처리
```cpp
scene.each<rex::Transform>([&](rex::EntityId id, rex::Transform& t) {
    // 시스템 단위 처리
});
```

권장사항:
- 한 `each<T>` 루프에서 과도한 할당 금지
- 중간 결과는 로컬 버퍼에 모아 다음 단계에서 적용

## 3. Components 상세
### 3.1 Transform
- position/rotation(deg)/scale
- `getMatrix()`는 Euler를 이용해 행렬 생성

### 3.2 MeshRenderer
- `mesh` 또는 `model`을 그리기 소스로 사용
- PBR 파라미터(metallic/roughness/ao) 포함

### 3.3 RigidBodyComponent
- 물리 입력 파라미터 + 내부 바디 포인터(`internalBody`)
- 내부 포인터는 PhysicsSystem이 소유/갱신한다.

### 3.4 Camera / Light
- Camera: LH projection (`perspectiveLH`)
- Light: Directional/Point 타입

## 4. RexMath 사용 포인트
### 4.1 Quat
- `Quat::fromEulerXYZ`, `toEulerXYZ`, `rotate`
- 물리 회전에 사용

### 4.2 Mat4
- `translate`, `rotateX/Y/Z`, `scale`
- `lookAtLH`, `perspectiveLH` 제공

### 4.3 주의사항
- Transform 회전은 degree, 물리 내부 회전 계산은 radian 기반 변환
- 호출 경계에서 단위를 명확히 유지

## 5. Window/Logger
### 5.1 Window
- SDL 초기화 + OpenGL 컨텍스트 생성
- `pollEvents`, `swapBuffers`, `setVSync`

### 5.2 Logger
- `std::format` 기반
- `trace/info/warn/error` 제공

## 6. 외부 개발자 체크리스트
- `Scene`과 `Components`만으로 최소 샘플 구성 가능한가
- 시스템 업데이트 순서를 문서대로 지키는가

## 7. 내부 개발자 체크리스트
- Core 헤더 변경 시 하위 모듈 컴파일 영향 검토
- API 시그니처 변경 시 문서/샘플 동시 갱신
