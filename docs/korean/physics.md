# Physics 모듈 상세 가이드

## 1. 구성
- `RigidBody`: 강체 상태/힘/임펄스 인터페이스
- `PhysicsWorld`: 시뮬레이션/충돌/제약/CCD
- `PhysicsSystem`: ECS <-> 물리 동기화

## 2. 시뮬레이션 파이프라인
`PhysicsWorld::step(dt)` -> 내부 고정 스텝 루프

서브스텝 단위 핵심 순서:
1. 시작 상태 저장
2. 적분 (선속도/각속도 + damping)
3. broadphase 갱신 (Dynamic AABB Tree)
4. TOI CCD 적용
5. narrowphase (SAT OBB + manifold 생성)
6. 웜스타트 + 속도 솔버
7. 위치 솔버
8. 캐시/슬립 상태 갱신

## 3. 현재 주요 기능
- Quat 기반 회전 동역학
- World inverse inertia 적용
- Dynamic AABB Tree broadphase
- Contact manifold (2~4점)
- Manifold warmstart/cache
- Distance Joint
- TOI 기반 CCD(스윕 구 근사)
- OBB raycast

## 4. 외부 개발자 사용법
### 4.1 컴포넌트 구성
```cpp
scene.addComponent<rex::RigidBodyComponent>(entity, rex::BodyType::Dynamic, 1.0f);
```

### 4.2 시스템 업데이트
```cpp
rex::PhysicsSystem physics;
physics.getWorld().setSolverIterations(12, 5);
physics.getWorld().setMaxSubSteps(8);

physics.update(scene, dt);
```

### 4.3 Joint 생성
- 먼저 `physics.update(scene, 0.0f)`로 내부 바디 생성
- `RigidBodyComponent.internalBody` 포인터 사용
- `DistanceJointDesc`로 조인트 등록

## 5. 튜닝 가이드
- 떨림 감소: velocity/position iteration 증가
- 관통 감소: maxSubSteps 증가 + CCD 활성화
- 과한 바운스: restitution 임계치/값 조정
- 스택 안정성: 마찰/슬립 파라미터 점검

## 6. 내부 개발자 확장 가이드
### 6.1 충돌 형태 확장
- broadphase proxy 전략 유지
- `buildManifold` 확장 시 캐시 키/웜스타트 호환 유지

### 6.2 제약 확장
- Joint는 준비/웜스타트/속도솔브/위치솔브 4단계 구조 유지
- 기존 solver iteration 루프에 통합

## 7. 디버깅 체크리스트
- body type/mass/invMass 일관성
- orientation 단위(rad/deg) 경계
- ccd flag 활성 상태
- 슬립 상태로 인해 업데이트가 멈춘 케이스
