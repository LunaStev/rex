#pragma once

#include <functional>
#include <optional>
#include <utility>

namespace rex::ui::framework::state {

template <typename TValue>
class Computed {
public:
    using ComputeFn = std::function<TValue()>;

    Computed();
    explicit Computed(ComputeFn fn);

    const TValue& value() const;
    void invalidate();
    void recompute();

private:
    mutable std::optional<TValue> cached_;
    mutable bool dirty_ = true;
    mutable bool computing_ = false;
    ComputeFn fn_;
};

// TODO [RexUI-Framework-State-002]:
// 책임: 파생 상태(Computed) 평가 모델 정의
// 요구사항:
//  - lazy/eager 계산 모드 지원
//  - 의존성 변경시 invalidate 전파
//  - 순환 의존 감지
// 의존성:
//  - State/Observable
// 구현 단계: Phase C
// 성능 고려사항:
//  - recompute 횟수 최소화
//  - graph invalidation 비용 제어
// 테스트 전략:
//  - 파생 상태 체인 정확성 테스트
//  - 순환 의존 오류 테스트

template <typename TValue>
Computed<TValue>::Computed() = default;

template <typename TValue>
Computed<TValue>::Computed(ComputeFn fn)
    : fn_(std::move(fn)) {}

template <typename TValue>
const TValue& Computed<TValue>::value() const {
    if (dirty_) {
        recompute();
    }
    if (!cached_.has_value()) {
        static TValue fallback{};
        return fallback;
    }
    return *cached_;
}

template <typename TValue>
void Computed<TValue>::invalidate() {
    dirty_ = true;
}

template <typename TValue>
void Computed<TValue>::recompute() {
    if (!fn_ || computing_) return;
    computing_ = true;
    cached_ = fn_();
    dirty_ = false;
    computing_ = false;
}

} // namespace rex::ui::framework::state
