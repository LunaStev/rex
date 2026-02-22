#pragma once

namespace rex::core::time {

struct TimeState {
    float rawDeltaTime = 0.0f;
    float deltaTime = 0.0f;
    float fixedDeltaTime = 1.0f / 60.0f;
    float timeScale = 1.0f;
    float totalTime = 0.0f;
    bool paused = false;
};

// TODO [Core-Time-001]:
// 책임: 엔진 시간 상태 모델 제공
// 요구사항:
//  - raw/scaled/fixed delta 보관
//  - timeScale 및 pause 상태 포함
//  - 프레임 누적 시간 추적
// 의존성:
//  - 없음
// 구현 단계: Phase A
// 성능 고려사항:
//  - trivial 타입 유지
//  - 핫패스 접근 최적화
// 테스트 전략:
//  - timeScale 적용 테스트
//  - pause 상태 테스트

} // namespace rex::core::time

