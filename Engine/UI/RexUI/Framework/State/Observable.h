#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <utility>

namespace rex::ui::framework::state {

template <typename TValue>
class Observable {
public:
    using SubscriptionId = std::uint64_t;
    using Callback = std::function<void(const TValue&)>;

    Observable();
    explicit Observable(TValue initialValue);

    const TValue& value() const;
    void setValue(const TValue& nextValue);

    SubscriptionId subscribe(Callback callback);
    void unsubscribe(SubscriptionId id);

private:
    TValue value_{};
    SubscriptionId nextSubscriptionId_ = 1;
    std::unordered_map<SubscriptionId, Callback> subscribers_;
};

// TODO [RexUI-Framework-State-001]:
// 책임: 관찰 가능한 상태 단위(Observable) 정의
// 요구사항:
//  - 구독/해지 API 계약
//  - 변경 통지 순서 결정성
//  - 불변성/값 비교 정책 정의
// 의존성:
//  - 없음
// 구현 단계: Phase C
// 성능 고려사항:
//  - 구독자 수 증가 시 notify 비용 관리
//  - 불필요한 동일 값 알림 방지
// 테스트 전략:
//  - 구독/해지/중첩 알림 테스트
//  - 경쟁 조건 시나리오 테스트

template <typename TValue>
Observable<TValue>::Observable() = default;

template <typename TValue>
Observable<TValue>::Observable(TValue initialValue)
    : value_(std::move(initialValue)) {}

template <typename TValue>
const TValue& Observable<TValue>::value() const {
    return value_;
}

template <typename TValue>
void Observable<TValue>::setValue(const TValue& nextValue) {
    if constexpr (requires (const TValue& a, const TValue& b) { a == b; }) {
        if (value_ == nextValue) return;
    }

    value_ = nextValue;
    for (const auto& [id, cb] : subscribers_) {
        (void)id;
        if (cb) cb(value_);
    }
}

template <typename TValue>
typename Observable<TValue>::SubscriptionId Observable<TValue>::subscribe(Callback callback) {
    const SubscriptionId id = nextSubscriptionId_++;
    subscribers_[id] = std::move(callback);
    return id;
}

template <typename TValue>
void Observable<TValue>::unsubscribe(SubscriptionId id) {
    subscribers_.erase(id);
}

} // namespace rex::ui::framework::state
