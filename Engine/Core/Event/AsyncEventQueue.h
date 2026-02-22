#pragma once

#include <mutex>
#include <queue>

#include "EventBus.h"

namespace rex::core::event {

class AsyncEventQueue {
public:
    void enqueue(const EngineEvent& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(event);
    }

    void flush(EventBus& bus) {
        std::queue<EngineEvent> local;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::swap(local, queue_);
        }
        while (!local.empty()) {
            bus.publish(local.front());
            local.pop();
        }
    }

private:
    std::mutex mutex_;
    std::queue<EngineEvent> queue_;
};

// TODO [Core-Event-003]:
// 책임: 비동기 이벤트 큐잉과 프레임 단위 flush 제공
// 요구사항:
//  - 멀티스레드 enqueue 안전성
//  - 메인 스레드 flush
//  - 이벤트 폭주(backpressure) 확장 포인트
// 의존성:
//  - Event/EventBus
// 구현 단계: Phase D
// 성능 고려사항:
//  - lock 구간 최소화
//  - flush 시 복사/이동 비용 제어
// 테스트 전략:
//  - 멀티스레드 enqueue 테스트
//  - flush 순서 테스트

} // namespace rex::core::event

