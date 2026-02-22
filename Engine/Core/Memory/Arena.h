#pragma once

#include <cstddef>
#include <vector>

#include "IAllocator.h"

namespace rex::core::memory {

class Arena final : public IAllocator {
public:
    explicit Arena(IAllocator* backing)
        : backing_(backing) {}

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override {
        if (!backing_) return nullptr;
        void* ptr = backing_->allocate(size, alignment);
        if (ptr) {
            allocations_.push_back({ptr, size});
        }
        return ptr;
    }

    void deallocate(void* ptr, std::size_t size = 0) override {
        if (!backing_ || !ptr) return;
        backing_->deallocate(ptr, size);
    }

    void reset() override {
        if (!backing_) return;
        for (auto it = allocations_.rbegin(); it != allocations_.rend(); ++it) {
            backing_->deallocate(it->ptr, it->size);
        }
        allocations_.clear();
    }

private:
    struct Allocation {
        void* ptr = nullptr;
        std::size_t size = 0;
    };

    IAllocator* backing_ = nullptr;
    std::vector<Allocation> allocations_;
};

// TODO [Core-Memory-005]:
// 책임: 상위 allocator 위에 범위 기반 할당 컨텍스트 제공
// 요구사항:
//  - arena 수명주기 동안 할당 추적
//  - reset 시 일괄 해제
//  - backing allocator 위임
// 의존성:
//  - Memory/IAllocator
// 구현 단계: Phase B
// 성능 고려사항:
//  - 추적 테이블 재할당 최소화
//  - reset 비용 예측 가능성
// 테스트 전략:
//  - 대량 allocate/reset 테스트
//  - backing allocator 결합 테스트

} // namespace rex::core::memory

