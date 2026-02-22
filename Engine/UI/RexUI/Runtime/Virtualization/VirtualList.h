#pragma once

#include <cstdint>

namespace rex::ui::runtime::virtualization {

class VirtualList {
public:
    void setItemCount(std::uint64_t itemCount);
    void setItemExtent(float itemExtent);
    void setViewport(float offset, float extent);

    std::uint64_t firstVisibleIndex() const;
    std::uint64_t visibleCount() const;

private:
    std::uint64_t itemCount_ = 0;
    float itemExtent_ = 1.0f;
    float viewportOffset_ = 0.0f;
    float viewportExtent_ = 0.0f;
};

// TODO [RexUI-Runtime-Virtualization-001]:
// 책임: 리스트 가상화(보이는 항목만 realize)
// 요구사항:
//  - 스크롤 오프셋 기반 visible range 계산
//  - 아이템 재사용 풀 연동 포인트
//  - overscan 정책
// 의존성:
//  - 없음
// 구현 단계: Phase E
// 성능 고려사항:
//  - 스크롤 중 GC/할당 최소화
//  - visible range 계산 O(1)
// 테스트 전략:
//  - 대량 항목(50k+) 범위 계산 테스트
//  - overscan 경계 테스트

} // namespace rex::ui::runtime::virtualization
