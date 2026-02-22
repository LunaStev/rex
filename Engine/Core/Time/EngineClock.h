#pragma once

#include <algorithm>
#include <chrono>

#include "TimeState.h"

namespace rex::core::time {

class EngineClock {
public:
    using Clock = std::chrono::steady_clock;

    void reset() {
        started_ = false;
        state_ = {};
    }

    void setTimeScale(float scale) {
        state_.timeScale = std::max(0.0f, scale);
    }

    void setPaused(bool paused) {
        state_.paused = paused;
    }

    const TimeState& tick() {
        const auto now = Clock::now();
        if (!started_) {
            last_ = now;
            started_ = true;
            state_.rawDeltaTime = 0.0f;
            state_.deltaTime = 0.0f;
            return state_;
        }

        const std::chrono::duration<float> dt = now - last_;
        last_ = now;
        state_.rawDeltaTime = dt.count();
        state_.deltaTime = state_.paused ? 0.0f : (state_.rawDeltaTime * state_.timeScale);
        state_.totalTime += state_.deltaTime;
        return state_;
    }

    const TimeState& state() const {
        return state_;
    }

private:
    bool started_ = false;
    Clock::time_point last_{};
    TimeState state_{};
};

// TODO [Core-Time-002]:
// 책임: 실시간 시계 기반 deltaTime 계산
// 요구사항:
//  - tick 기반 raw/scaled delta 산출
//  - pause/timeScale 반영
//  - 초기 프레임 안정화
// 의존성:
//  - Time/TimeState
// 구현 단계: Phase A
// 성능 고려사항:
//  - tick 경로 분기 최소화
//  - 고해상도 클록 호출 비용 관리
// 테스트 전략:
//  - tick 누적 시간 테스트
//  - pause/resume 시나리오 테스트

} // namespace rex::core::time

