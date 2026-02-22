#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "ThreadPool.h"

namespace rex::core::job {

class TaskGraph {
public:
    using TaskId = std::uint64_t;
    using TaskFn = std::function<void()>;

    TaskId addTask(TaskFn fn, std::vector<TaskId> dependencies = {}) {
        const TaskId id = nextTaskId_++;
        tasks_[id] = {std::move(fn), std::move(dependencies)};
        return id;
    }

    void clear() {
        tasks_.clear();
    }

    void execute(ThreadPool& pool) {
        std::unordered_map<TaskId, bool> completed;
        completed.reserve(tasks_.size());

        while (completed.size() < tasks_.size()) {
            bool progressed = false;
            for (auto& [id, task] : tasks_) {
                if (completed[id]) continue;

                bool ready = true;
                for (TaskId dep : task.dependencies) {
                    if (!completed[dep]) {
                        ready = false;
                        break;
                    }
                }
                if (!ready) continue;

                auto fut = pool.submit([fn = task.fn]() {
                    if (fn) fn();
                });
                fut.get();
                completed[id] = true;
                progressed = true;
            }

            if (!progressed) {
                break;
            }
        }
    }

private:
    struct TaskNode {
        TaskFn fn{};
        std::vector<TaskId> dependencies{};
    };

    TaskId nextTaskId_ = 1;
    std::unordered_map<TaskId, TaskNode> tasks_;
};

// TODO [Core-Job-004]:
// 책임: 작업 의존 그래프(Task Graph) 실행 모델 제공
// 요구사항:
//  - task/dependency 등록
//  - ready task 스케줄링
//  - 순환 의존 감지 확장 포인트
// 의존성:
//  - Job/ThreadPool
// 구현 단계: Phase B
// 성능 고려사항:
//  - ready queue 증분 계산
//  - 대형 그래프 O(N+E) 실행 목표
// 테스트 전략:
//  - 의존 순서 테스트
//  - 순환/누락 의존성 테스트

} // namespace rex::core::job

