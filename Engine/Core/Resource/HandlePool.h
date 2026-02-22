#pragma once

#include <cstdint>
#include <vector>

#include "Handle.h"

namespace rex::core::resource {

template <typename TTag>
class HandlePool {
public:
    StrongHandle<TTag> allocate() {
        std::uint32_t index = 0;
        if (!freeList_.empty()) {
            index = freeList_.back();
            freeList_.pop_back();
        } else {
            index = static_cast<std::uint32_t>(generations_.size());
            generations_.push_back(1);
        }

        return StrongHandle<TTag>{{index, generations_[index]}};
    }

    void release(StrongHandle<TTag> handle) {
        if (!isValid(handle)) return;
        ++generations_[handle.id.index];
        freeList_.push_back(handle.id.index);
    }

    bool isValid(StrongHandle<TTag> handle) const {
        if (!handle) return false;
        if (handle.id.index >= generations_.size()) return false;
        return generations_[handle.id.index] == handle.id.generation;
    }

    WeakHandle<TTag> weaken(StrongHandle<TTag> handle) const {
        return WeakHandle<TTag>{handle.id};
    }

    StrongHandle<TTag> lock(WeakHandle<TTag> handle) const {
        StrongHandle<TTag> strong{handle.id};
        return isValid(strong) ? strong : StrongHandle<TTag>{};
    }

private:
    std::vector<std::uint32_t> generations_;
    std::vector<std::uint32_t> freeList_;
};

// TODO [Core-Resource-002]:
// 책임: 핸들 발급/해제/검증 풀 제공
// 요구사항:
//  - generation 기반 유효성 검사
//  - free list 재사용
//  - weak -> strong 승격(lock) API
// 의존성:
//  - Resource/Handle
// 구현 단계: Phase D
// 성능 고려사항:
//  - allocate/release O(1)
//  - generation overflow 정책 정의
// 테스트 전략:
//  - release 후 stale handle 테스트
//  - weak lock 유효성 테스트

} // namespace rex::core::resource

