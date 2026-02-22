#pragma once

#include <deque>
#include <mutex>
#include <optional>

namespace rex::core::job {

template <typename T>
class WorkStealingQueue {
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push_back(std::move(value));
    }

    std::optional<T> tryPop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return std::nullopt;
        T value = std::move(queue_.back());
        queue_.pop_back();
        return value;
    }

    std::optional<T> trySteal() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return std::nullopt;
        T value = std::move(queue_.front());
        queue_.pop_front();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    mutable std::mutex mutex_;
    std::deque<T> queue_;
};

// TODO [Core-Job-002]:
// 책임: per-worker work stealing 큐 제공
// 요구사항:
//  - owner pop / thief steal API
//  - lock-free 또는 저경합 구조 확장 가능
//  - empty 관찰 API
// 의존성:
//  - Job/SyncPrimitives
// 구현 단계: Phase B
// 성능 고려사항:
//  - steal 경로 경합 최소화
//  - 대량 태스크 push/pop 효율
// 테스트 전략:
//  - 멀티스레드 pop/steal 테스트
//  - 큐 무결성 테스트

} // namespace rex::core::job

