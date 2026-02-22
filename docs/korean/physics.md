# Physics 모듈 상세 가이드

## 1. 아키텍처
현재 물리 런타임은 다음 구조로 동작한다.
- `RigidBody` (게임플레이/ECS에서 사용하는 C++ 핸들)
- `PhysicsSystem` (ECS 동기화, 고정 스텝 루프, 외부 API)
- Rust 코어(`Engine/Rust/physics_core`) + FFI 브리지(`RustPhysicsFFI.h`)

기존 C++ `PhysicsWorld` 구현은 Rust 코어 경로로 대체되었다.

## 2. 런타임 동작 흐름
`PhysicsSystem::update(scene, dt)`가 전체 시뮬레이션을 구동한다.

상위 흐름:
1. ECS 컴포넌트 -> 내부 바디 핸들 동기화
2. 고정 스텝 누적 처리
3. FFI를 통한 Rust 월드 스텝 호출
4. 조인트/제약 상태 갱신
5. 계산 결과를 ECS로 다시 반영

## 3. 현재 경로 기능
- 강체 시뮬레이션(dynamic/kinematic/static)
- 선속도/각속도 적분
- 중력, damping, 마찰/탄성 파라미터
- 런타임 기준 CCD 경로 활성
- Distance Joint 등록/삭제/관리
- `PhysicsSystem::raycast` API

## 4. 외부 개발자 사용법
### 4.1 시스템 설정과 업데이트
```cpp
rex::PhysicsSystem physics;
physics.setGravity({0.0f, -9.81f, 0.0f});
physics.setSolverIterations(14, 6);
physics.setMaxSubSteps(8);

physics.update(scene, dt);
```

### 4.2 강체 컴포넌트 추가
```cpp
auto& rb = scene.addComponent<rex::RigidBodyComponent>(entity, rex::BodyType::Dynamic, 1.0f);
rb.enableCCD = true;
rb.restitution = 0.2f;
rb.staticFriction = 0.7f;
rb.dynamicFriction = 0.5f;
```

### 4.3 Distance Joint 추가
```cpp
rex::DistanceJointDesc j;
j.bodyA = rbA->internalBody;
j.bodyB = rbB->internalBody;
j.restLength = 1.8f;
j.stiffness = 0.8f;
j.damping = 0.2f;

int id = physics.addDistanceJoint(j);
```

주의: 내부 바디가 아직 생성되지 않았다면, 먼저 `physics.update(scene, 0.0f)`를 1회 호출한다.

## 5. 튜닝 가이드
- 스택 떨림: solver iteration 증가 + damping 조정
- 관통: CCD 활성 유지 + maxSubSteps 증가
- 충돌 폭주: restitution/임펄스 강도 완화
- 체인 불안정: stiffness 하향 또는 damping 상향

## 6. 내부 확장 가이드
- FFI 경계는 명시적이고 데이터 지향적으로 유지
- C++/Rust 소유권 경계 모호성 제거
- 내부 Rust 모듈 분할 시 `RigidBody` 외부 API 안정성 유지
- 대규모 엔티티 씬에서 동기화 비용 계측 유지

## 7. 디버그 체크리스트
- `RigidBodyComponent.internalBody` 연결 확인 후 임펄스/조인트 사용
- 테스트 씬에 맞는 gravity/solver/substep 설정 확인
- 런타임 dt 스파이크 clamp 여부 확인
- raycast 호출 시 direction 정규화 확인
