#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "IAllocator.h"

namespace rex::core::memory {

class LinearAllocator final : public IAllocator {
public:
    explicit LinearAllocator(std::size_t capacityBytes = 0)
        : buffer_(capacityBytes)
        , offset_(0) {}

    void reserve(std::size_t capacityBytes) {
        buffer_.resize(capacityBytes);
        offset_ = std::min(offset_, capacityBytes);
    }

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override {
        if (size == 0 || buffer_.empty()) return nullptr;

        std::uintptr_t base = reinterpret_cast<std::uintptr_t>(buffer_.data());
        std::uintptr_t current = base + offset_;
        const std::size_t mask = alignment - 1;
        std::uintptr_t aligned = (current + mask) & ~static_cast<std::uintptr_t>(mask);
        const std::size_t alignedOffset = static_cast<std::size_t>(aligned - base);
        if (alignedOffset + size > buffer_.size()) {
            return nullptr;
        }

        offset_ = alignedOffset + size;
        return reinterpret_cast<void*>(aligned);
    }

    void deallocate(void* ptr, std::size_t size = 0) override {
        (void)ptr;
        (void)size;
        // 선형 할당기는 개별 free를 지원하지 않는다.
    }

    void reset() override {
        offset_ = 0;
    }

    std::size_t usedBytes() const {
        return offset_;
    }

    std::size_t capacityBytes() const {
        return buffer_.size();
    }

private:
    std::vector<std::byte> buffer_;
    std::size_t offset_ = 0;
};

// TODO [Core-Memory-002]:
// 책임: 선형(스택형) 할당기 제공
// 요구사항:
//  - bump-pointer allocate
//  - 프레임 단위 reset
//  - 개별 deallocate 미지원 정책 명시
// 의존성:
//  - Memory/IAllocator
// 구현 단계: Phase B
// 성능 고려사항:
//  - allocate O(1)
//  - alignment 계산 분기 최소화
// 테스트 전략:
//  - capacity 초과 테스트
//  - alignment 정확성 테스트

} // namespace rex::core::memory

