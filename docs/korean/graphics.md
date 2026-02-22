# Graphics 모듈 가이드 (내부/외부 개발자 공용)

## 1. 범위
Graphics 모듈은 더 이상 단일 렌더러가 아니라, 모듈화된 실시간 렌더링 파이프라인으로 구성된다.

핵심 목표:
- 비주얼 퀄리티 향상(PBR + HDR + Post Process)
- 유지보수를 위한 모듈 경계 분리
- 다광원 씬 확장성 확보

## 2. 상위 아키텍처
외부 API는 여전히 `rex::Renderer`를 사용하지만, 내부는 Deferred 파이프라인으로 실행된다.

주요 패스 순서:
1. `ShadowPass`
2. `GBufferPass`
3. `LightingPass`
4. `PostProcessPass`
5. `UIPass` (향후 UI 합성 지점)

관련 코드:
- `Engine/Graphics/Renderer.h`
- `Engine/Graphics/Pipeline/DeferredPipeline.h`
- `Engine/Graphics/Core/RenderGraph.h`

## 3. 모듈 분리 구조
```text
Engine/Graphics/
  Core/
    RenderDevice.*
    RenderPass.h
    RenderGraph.*
    FrameBuffer.*
  Lighting/
    Light.h
    LightManager.*
    ShadowSystem.*
  Material/
    Material.h
    PBRMaterial.h
    ShaderVariant.h
  Pipeline/
    DeferredPipeline.*
    ForwardPipeline.*
    PostProcessPipeline.*
  PostProcess/
    SSAOPass.*
    BloomPass.*
    ToneMappingPass.*
  HDR/
    HDRBuffer.*
    ExposureController.*
  Culling/
    FrustumCuller.*
    LightCuller.*
```

## 4. 광원 모델
`Engine/Core/Components.h`의 `Light` 타입 지원:
- Directional
- Point
- Spot
- Area

광원 속성:
- 선형 공간 `color`
- `intensity`
- `castShadows`
- 감쇠/거리 제어
- 스팟 각도(`innerConeDeg`, `outerConeDeg`)
- `volumetric` 플래그(데이터 경로만 존재, 완전한 볼류메트릭 패스는 미완료)

## 5. 셰이딩 파이프라인
### 5.1 GBuffer 구성
- RT0: 월드 위치
- RT1: 노말 + 거칠기
- RT2: 알베도 + 메탈릭
- RT3: AO

### 5.2 조명 모델
- Cook-Torrance BRDF
- GGX NDF + Smith Geometry + Schlick Fresnel
- Metallic/Roughness 워크플로

### 5.3 HDR/컬러
- HDR 라이팅 타겟
- SSAO, Bloom 독립 패스
- ACES 필믹 톤매핑
- 최종 감마 보정

## 6. 그림자
Directional 광원은 Cascaded Shadow Map + Atlas 기반으로 처리된다.

현재 구현:
- 4 cascade
- atlas 분할
- 라이팅 패스 PCF 샘플링
- slope 기반 depth bias

## 7. 런타임 연동
기본 API는 동일하다:
```cpp
renderer.render(scene, camera, view, viewPos, width, height, backbufferFBO);
```

후처리 런타임 설정 예시:
```cpp
auto& pp = renderer.deferredPipeline().postProcess().settings();
pp.enableBloom = true;
pp.autoExposure = true;
pp.exposure = 1.15f;
pp.bloomStrength = 0.14f;
```

## 8. 런타임 샌드박스 구성
`Engine/Runtime/runtime_main.cpp`는 파이프라인 검증용으로 다음을 포함한다.
- 혼합 광원 타입
- 그림자 투영 Directional key light
- 애니메이션 point/spot light
- 128개 stress point light
- bloom/exposure 실시간 토글

렌더링/물리 통합 테스트 기준 파일로 이 엔트리를 사용한다.

## 9. 성능 메모
현재 반영:
- 가시 렌더러블 필터링(프러스텀 기반)
- CPU-side light ranking/culling
- FBO 재사용 기반 RT 재할당 최소화

다음 단계:
- Forward+ tile/cluster GPU light culling
- Render Graph 리소스 aliasing 강화
- 오클루전 전략 확장

## 10. 트러블슈팅
- 검은 화면:
  - GL 함수 로딩 확인 (`Engine/Graphics/GLInternal.h`)
  - FBO complete 상태 확인 (`FrameBuffer`)
- 그림자 없음:
  - Directional light에서 `castShadows = true` 설정 확인
- 화면이 밋밋함:
  - exposure/bloom 값 확인
  - `MeshRenderer`의 roughness/metallic 값 점검
- 다광원 성능 저하:
  - stress light 수/범위 축소
  - 런타임 샌드박스에서 `K`로 애니메이션 비활성화
