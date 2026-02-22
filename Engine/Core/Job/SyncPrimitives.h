#pragma once

#include <atomic>
#include <barrier>
#include <condition_variable>
#include <mutex>

namespace rex::core::job {

using Mutex = std::mutex;
using Condition = std::condition_variable;
using Barrier = std::barrier<>;

class Spinlock {
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
        }
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

// TODO [Core-Job-001]:
// 책임: 잡 시스템 공통 동기화 프리미티브 제공
// 요구사항:
//  - Mutex/Condition/Barrier/Spinlock 제공
//  - 표준 인터페이스와 호환
//  - 데드락 디버깅 훅 확장 포인트
// 의존성:
//  - 없음
// 구현 단계: Phase B
// 성능 고려사항:
//  - lock 경합 구간 최소화
//  - spin 대기 백오프 전략
// 테스트 전략:
//  - 경쟁 조건 스트레스 테스트
//  - barrier 동기화 테스트

} // namespace rex::core::job

