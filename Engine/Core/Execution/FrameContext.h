#pragma once

#include <cstdint>

namespace rex::core::execution {

enum class FramePhase : std::uint8_t {
    PreUpdate = 0,
    Update,
    PostUpdate,
    PreRender,
    Render,
    PostRender,
    Count
};

struct FrameContext {
    std::uint64_t frameIndex = 0;
    float deltaTime = 0.0f;
    float fixedDeltaTime = 1.0f / 60.0f;
    float interpolationAlpha = 0.0f;
    float timeScale = 1.0f;
};

// TODO [Core-Execution-001]:
// 책임: 프레임 실행 공통 컨텍스트와 페이즈 모델 정의
// 요구사항:
//  - 프레임 인덱스/시간 값 전달
//  - 페이즈 enum 고정
//  - 런타임/에디터 공통 사용
// 의존성:
//  - 없음
// 구현 단계: Phase A
// 성능 고려사항:
//  - hot path에서 trivially-copyable 유지
//  - 캐시 친화적 필드 배치
// 테스트 전략:
//  - 프레임 값 전달 무결성 테스트
//  - 페이즈 순서 회귀 테스트

} // namespace rex::core::execution

