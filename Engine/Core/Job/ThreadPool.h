#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace rex::core::job {

class ThreadPool {
public:
    ThreadPool() = default;

    explicit ThreadPool(std::size_t threadCount) {
        start(threadCount);
    }

    ~ThreadPool() {
        stop();
    }

    void start(std::size_t threadCount = std::thread::hardware_concurrency()) {
        if (!workers_.empty()) return;
        stopRequested_ = false;
        const std::size_t count = threadCount == 0 ? 1 : threadCount;
        workers_.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            workers_.emplace_back([this]() {
                workerLoop();
            });
        }
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopRequested_ = true;
        }
        cv_.notify_all();
        for (auto& worker : workers_) {
            if (worker.joinable()) worker.join();
        }
        workers_.clear();
    }

    template <typename Fn, typename... Args>
    auto submit(Fn&& fn, Args&&... args)
        -> std::future<std::invoke_result_t<Fn, Args...>> {
        using ResultT = std::invoke_result_t<Fn, Args...>;
        auto task = std::make_shared<std::packaged_task<ResultT()>>(
            std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
        std::future<ResultT> future = task->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push([task]() { (*task)(); });
        }
        cv_.notify_one();
        return future;
    }

    std::size_t workerCount() const {
        return workers_.size();
    }

private:
    void workerLoop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]() {
                    return stopRequested_ || !tasks_.empty();
                });
                if (stopRequested_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            if (task) task();
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::condition_variable cv_;
    std::mutex mutex_;
    bool stopRequested_ = false;
};

// TODO [Core-Job-003]:
// 책임: 공용 스레드풀 실행기 제공
// 요구사항:
//  - start/stop lifecycle
//  - future 기반 submit
//  - graceful shutdown 보장
// 의존성:
//  - Job/SyncPrimitives
// 구현 단계: Phase B
// 성능 고려사항:
//  - 태스크 enqueue/dequeue 오버헤드 최소화
//  - 스레드 생성/파괴 비용 관리
// 테스트 전략:
//  - 다중 태스크 결과 검증
//  - stop 중 제출 경계 테스트

} // namespace rex::core::job
