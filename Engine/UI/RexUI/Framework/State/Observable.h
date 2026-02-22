#pragma once

#include <cstdint>
#include <functional>

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

} // namespace rex::ui::framework::state
