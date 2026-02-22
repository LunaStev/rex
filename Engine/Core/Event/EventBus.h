#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "EngineEvents.h"

namespace rex::core::event {

class EventBus {
public:
    using SubscriptionId = std::uint64_t;
    using Handler = std::function<void(const EngineEvent&)>;

    SubscriptionId subscribe(Handler handler) {
        const SubscriptionId id = nextId_++;
        handlers_[id] = std::move(handler);
        return id;
    }

    void unsubscribe(SubscriptionId id) {
        handlers_.erase(id);
    }

    void publish(const EngineEvent& event) const {
        for (const auto& [id, handler] : handlers_) {
            (void)id;
            if (handler) handler(event);
        }
    }

private:
    SubscriptionId nextId_ = 1;
    std::unordered_map<SubscriptionId, Handler> handlers_;
};

// TODO [Core-Event-002]:
// 책임: 엔진 전역 이벤트 발행/구독 버스 제공
// 요구사항:
//  - subscribe/unsubscribe/publish API
//  - 핸들러 실패 격리 정책
//  - 타입 필터링 확장 포인트
// 의존성:
//  - Event/EngineEvents
// 구현 단계: Phase D
// 성능 고려사항:
//  - publish 경로 O(N) 최적화
//  - 핸들러 등록/해제 비용 최소화
// 테스트 전략:
//  - 구독/해지 동작 테스트
//  - 다중 핸들러 호출 테스트

} // namespace rex::core::event

