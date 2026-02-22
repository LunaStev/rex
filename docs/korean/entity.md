# Entity/Component 운용 가이드

Rex는 ECS 패턴을 사용하므로, 엔티티는 ID이고 로직은 시스템이 처리한다.

## 1. 기본 패턴
### 1.1 엔티티 생성
```cpp
rex::EntityId e = scene.createEntity();
scene.addComponent<rex::Transform>(e, rex::Vec3{0, 0, 0});
scene.addComponent<rex::MeshRenderer>(e, nullptr, cube, rex::Vec3{1,1,1});
```

### 1.2 엔티티 삭제
```cpp
scene.destroyEntity(e);
```

## 2. 실전 아키타입 예시
### 2.1 동적 박스
- `Transform`
- `MeshRenderer`
- `RigidBodyComponent`(Dynamic)

### 2.2 정적 바닥
- `Transform` (큰 scale)
- `MeshRenderer`
- `RigidBodyComponent`(Static)

### 2.3 라이트
- `Transform`
- `Light`

## 3. 외부 개발자용 권장 흐름
1. 엔티티 생성
2. 컴포넌트 부착
3. 시스템 루프 실행
4. UI/에디터에서 선택/수정

## 4. 내부 개발자용 확장 방법
### 4.1 새 컴포넌트 추가
1. `Engine/Core/Components.h`에 구조체 추가
2. 시스템에서 `each<NewComponent>` 처리 추가
3. 에디터 Inspector 노출(필요 시)

### 4.2 새 시스템 추가
- 입력: `Scene&`, `dt`
- 출력: 컴포넌트 갱신
- 규칙: 시스템끼리 강결합 최소화

## 5. 안전한 접근 규칙
- `getComponent<T>` 반환 포인터는 null 가능
- 삭제된 엔티티 캐시를 장시간 보관하지 말 것

## 6. 흔한 실수
- `RigidBodyComponent.internalBody`를 외부에서 직접 소유/삭제
- 물리 동기화 이전에 Transform을 덮어써서 떨림 유발
- 컴포넌트 없는 엔티티를 전제한 코드

## 7. 디버깅 팁
- 선택 엔티티 ID 출력
- 컴포넌트 존재 여부 로그
- 물리 동기화 전/후 Transform 비교
