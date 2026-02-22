#pragma once

#include <cstddef>

namespace rex::core::memory {

class IAllocator {
public:
    virtual ~IAllocator() = default;
    virtual void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) = 0;
    virtual void deallocate(void* ptr, std::size_t size = 0) = 0;
    virtual void reset() {}
};

// TODO [Core-Memory-001]:
// 책임: 메모리 할당기 공통 인터페이스 정의
// 요구사항:
//  - allocate/deallocate 계약
//  - alignment 파라미터 지원
//  - reset 가능한 allocator 확장
// 의존성:
//  - 없음
// 구현 단계: Phase B
// 성능 고려사항:
//  - 가상 호출 오버헤드 최소화
//  - 정렬 할당 비용 제어
// 테스트 전략:
//  - 정렬/해제 무결성 테스트
//  - allocator 교체 테스트

} // namespace rex::core::memory

