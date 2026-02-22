#pragma once

#include <algorithm>
#include <cstdint>

#include "FrameContext.h"
#include "PhaseScheduler.h"

namespace rex::core::execution {

class IEngineLoopDelegate {
public:
    virtual ~IEngineLoopDelegate() = default;
    virtual bool onInit() = 0;
    virtual void onShutdown() = 0;
    virtual bool shouldClose() const = 0;
};

struct EngineLoopConfig {
    float fixedDeltaTime = 1.0f / 60.0f;
    float maxFrameTime = 0.1f;
    float timeScale = 1.0f;
};

class EngineLoop {
public:
    explicit EngineLoop(EngineLoopConfig config = {})
        : config_(config) {}

    bool init(IEngineLoopDelegate& delegate) {
        if (initialized_) return true;
        initialized_ = delegate.onInit();
        return initialized_;
    }

    void shutdown(IEngineLoopDelegate& delegate) {
        if (!initialized_) return;
        delegate.onShutdown();
        initialized_ = false;
    }

    bool runFrame(IEngineLoopDelegate& delegate, PhaseScheduler& scheduler, float rawDeltaTime) {
        if (!initialized_ && !init(delegate)) return false;
        if (delegate.shouldClose()) return false;

        const float scaledDt = std::clamp(rawDeltaTime, 0.0f, config_.maxFrameTime) * config_.timeScale;
        accumulator_ += scaledDt;

        FrameContext ctx{};
        ctx.frameIndex = frameIndex_;
        ctx.deltaTime = scaledDt;
        ctx.fixedDeltaTime = config_.fixedDeltaTime;
        ctx.timeScale = config_.timeScale;

        while (accumulator_ >= config_.fixedDeltaTime) {
            ctx.interpolationAlpha = 0.0f;
            scheduler.run(FramePhase::PreUpdate, ctx);
            scheduler.run(FramePhase::Update, ctx);
            scheduler.run(FramePhase::PostUpdate, ctx);
            accumulator_ -= config_.fixedDeltaTime;
        }

        ctx.interpolationAlpha = (config_.fixedDeltaTime > 0.0f)
            ? (accumulator_ / config_.fixedDeltaTime)
            : 0.0f;
        scheduler.run(FramePhase::PreRender, ctx);
        scheduler.run(FramePhase::Render, ctx);
        scheduler.run(FramePhase::PostRender, ctx);

        ++frameIndex_;
        return true;
    }

    std::uint64_t frameIndex() const {
        return frameIndex_;
    }

    const EngineLoopConfig& config() const {
        return config_;
    }

private:
    EngineLoopConfig config_{};
    bool initialized_ = false;
    std::uint64_t frameIndex_ = 0;
    float accumulator_ = 0.0f;
};

// TODO [Core-Execution-003]:
// 책임: 고정/가변 타임스텝을 포함한 엔진 루프 구동
// 요구사항:
//  - init/update/render/shutdown 수명주기
//  - fixed timestep accumulator
//  - phase scheduler 연동
// 의존성:
//  - Execution/FrameContext
//  - Execution/PhaseScheduler
// 구현 단계: Phase A
// 성능 고려사항:
//  - 프레임당 불필요 할당 0
//  - 큰 dt 입력 시 spiral-of-death 방지
// 테스트 전략:
//  - fixed step 회귀 테스트
//  - 종료 조건(shouldClose) 테스트

} // namespace rex::core::execution

