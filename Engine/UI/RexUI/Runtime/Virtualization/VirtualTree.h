#pragma once

#include <cstdint>

namespace rex::ui::runtime::virtualization {

class VirtualTree {
public:
    void setExpanded(std::uint64_t nodeId, bool expanded);
    bool isExpanded(std::uint64_t nodeId) const;

    void setViewport(float offset, float extent);
    std::uint64_t firstVisibleNode() const;
    std::uint64_t visibleNodeCount() const;
};

// TODO [RexUI-Runtime-Virtualization-002]:
// 책임: 트리 가상화 + 확장 상태 관리
// 요구사항:
//  - expanded/collapsed 상태 저장
//  - 평탄화(flatten) 인덱스 계산
//  - 보이는 노드만 realize
// 의존성:
//  - 없음
// 구현 단계: Phase E
// 성능 고려사항:
//  - 확장 토글시 재평탄화 비용 최소화
//  - 깊은 트리에서 visible 계산 최적화
// 테스트 전략:
//  - 확장/축소 연속 동작 테스트
//  - 대규모 트리 스크롤 테스트

} // namespace rex::ui::runtime::virtualization
