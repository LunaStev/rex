#pragma once

#include "LinearAllocator.h"

namespace rex::core::memory {

class FrameAllocator final : public IAllocator {
public:
    explicit FrameAllocator(std::size_t capacityBytes = 4 * 1024 * 1024)
        : linear_(capacityBytes) {}

    void beginFrame() {
        linear_.reset();
    }

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override {
        return linear_.allocate(size, alignment);
    }

    void deallocate(void* ptr, std::size_t size = 0) override {
        (void)ptr;
        (void)size;
    }

    void reset() override {
        linear_.reset();
    }

    std::size_t usedBytes() const {
        return linear_.usedBytes();
    }

private:
    LinearAllocator linear_;
};

// TODO [Core-Memory-003]:
// 책임: 프레임 경계 reset 기반 임시 메모리 할당기 제공
// 요구사항:
//  - beginFrame에서 전체 reset
//  - 대량 임시 객체 할당 경로 제공
//  - 용량 모니터링 API
// 의존성:
//  - Memory/LinearAllocator
// 구현 단계: Phase B
// 성능 고려사항:
//  - 프레임당 allocate O(1)
//  - zero-allocation frame 목표
// 테스트 전략:
//  - 프레임 경계 reset 테스트
//  - 초과 할당 실패 경로 테스트

} // namespace rex::core::memory

