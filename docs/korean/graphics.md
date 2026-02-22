# Graphics 모듈 상세 가이드

## 1. 책임
Graphics 모듈은 Scene 데이터를 화면으로 변환한다.

핵심 클래스:
- `Renderer`
- `Mesh`
- `Model`
- `Shader`

## 2. Renderer API
```cpp
void render(Scene& scene,
            const Camera& camera,
            const Mat4& viewMatrix,
            const Vec3& viewPos,
            int targetWidth,
            int targetHeight,
            uint32_t backbufferFBO,
            const std::function<void(const Mat4&, const Mat4&)>& extraDraw = {});
```

의미:
- `scene`: 렌더 대상 ECS
- `camera/viewMatrix/viewPos`: 시점 정보
- `targetWidth/targetHeight`: 출력 크기
- `backbufferFBO`: 출력 버퍼
- `extraDraw`: 커스텀 디버그/기즈모 드로우

## 3. 프레임 흐름(개념)
1. HDR 타겟 준비
2. 메시 렌더 패스
3. 톤매핑 패스
4. 추가 콜백 렌더

## 4. 외부 개발자 사용법
### 4.1 최소 샘플
```cpp
rex::Renderer renderer;
rex::Scene scene;
rex::Camera cam;
rex::Vec3 camPos{0,2,-8};
rex::Mat4 view = rex::Mat4::lookAtLH(camPos, {0,0,0}, {0,1,0});

renderer.render(scene, cam, view, camPos, width, height, 0);
```

### 4.2 메시 추가
- `Mesh::createCube()`로 테스트 지오메트리 생성
- `MeshRenderer`에 연결

## 5. 내부 개발자 확장 가이드
### 5.1 새 렌더 패스 추가
- `Renderer` 내부 패스 순서 정의
- 리소스 수명(재사용/해제) 명확화
- 디버그 토글로 on/off 가능하게 구현

### 5.2 셰이더 변경 정책
- 셰이더 인터페이스(유니폼 계약) 문서화
- 기존 머티리얼 파라미터와 호환성 유지

## 6. 성능 튜닝 포인트
- draw call 수
- state change 횟수
- GPU 버퍼 업로드 횟수
- HDR 타겟 재생성 빈도

## 7. 문제 해결
- 검은 화면: GL 초기화/컨텍스트 확인
- 깨진 조명: Light 컴포넌트 존재/값 확인
- 성능 급락: `extraDraw` 내 과도한 state 변경 점검
