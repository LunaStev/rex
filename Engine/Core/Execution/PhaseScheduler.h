#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <vector>

#include "FrameContext.h"

namespace rex::core::execution {

class PhaseScheduler {
public:
    using Callback = std::function<void(const FrameContext&)>;

    void registerPhaseCallback(FramePhase phase, Callback callback) {
        callbacks_[static_cast<std::size_t>(phase)].push_back(std::move(callback));
    }

    void clear() {
        for (auto& list : callbacks_) {
            list.clear();
        }
    }

    void run(FramePhase phase, const FrameContext& context) {
        auto& list = callbacks_[static_cast<std::size_t>(phase)];
        for (auto& cb : list) {
            if (cb) cb(context);
        }
    }

private:
    std::array<std::vector<Callback>, static_cast<std::size_t>(FramePhase::Count)> callbacks_{};
};

// TODO [Core-Execution-002]:
// 책임: 페이즈별 콜백 실행 순서 제어
// 요구사항:
//  - phase callback 등록/해제
//  - 페이즈별 결정적 실행 순서
//  - 런타임 도중 등록 확장 포인트
// 의존성:
//  - Execution/FrameContext
// 구현 단계: Phase A
// 성능 고려사항:
//  - 콜백 디스패치 오버헤드 최소화
//  - 불필요한 동적 할당 감소
// 테스트 전략:
//  - 실행 순서 고정 테스트
//  - 빈 페이즈 처리 테스트

} // namespace rex::core::execution

