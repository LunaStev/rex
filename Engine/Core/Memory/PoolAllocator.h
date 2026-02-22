#pragma once

#include <cstddef>
#include <vector>

#include "IAllocator.h"

namespace rex::core::memory {

class PoolAllocator final : public IAllocator {
public:
    PoolAllocator(std::size_t blockSize, std::size_t blockCount)
        : blockSize_(blockSize)
        , blockCount_(blockCount)
        , buffer_(blockSize * blockCount) {
        freeList_.reserve(blockCount);
        for (std::size_t i = 0; i < blockCount; ++i) {
            freeList_.push_back(buffer_.data() + (i * blockSize_));
        }
    }

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override {
        (void)alignment;
        if (size > blockSize_ || freeList_.empty()) return nullptr;
        void* ptr = freeList_.back();
        freeList_.pop_back();
        return ptr;
    }

    void deallocate(void* ptr, std::size_t size = 0) override {
        (void)size;
        if (!ptr) return;
        freeList_.push_back(static_cast<std::byte*>(ptr));
    }

    void reset() override {
        freeList_.clear();
        for (std::size_t i = 0; i < blockCount_; ++i) {
            freeList_.push_back(buffer_.data() + (i * blockSize_));
        }
    }

private:
    std::size_t blockSize_ = 0;
    std::size_t blockCount_ = 0;
    std::vector<std::byte> buffer_;
    std::vector<std::byte*> freeList_;
};

// TODO [Core-Memory-004]:
// 책임: 고정 크기 객체 풀 할당기 제공
// 요구사항:
//  - 동일 크기 블록 allocate/deallocate
//  - free-list 기반 재사용
//  - 전체 reset 지원
// 의존성:
//  - Memory/IAllocator
// 구현 단계: Phase B
// 성능 고려사항:
//  - allocate/deallocate O(1)
//  - 메모리 지역성 확보
// 테스트 전략:
//  - 블록 고갈/복구 테스트
//  - 잘못된 크기 요청 테스트

} // namespace rex::core::memory

