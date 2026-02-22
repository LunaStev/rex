#pragma once

#include <cstdint>

#include "ConstraintSolver.h"
#include "LayoutCache.h"

namespace rex::ui::runtime::tree {
class WidgetTree;
}

namespace rex::ui::runtime::layout {

class LayoutEngine {
public:
    LayoutEngine(ConstraintSolver* solver, LayoutCache* cache);

    void compute(tree::WidgetTree& tree, const core::LayoutConstraints& rootConstraints, std::uint64_t frameStamp);
};

// TODO [RexUI-Runtime-Layout-003]:
// 책임: WidgetTree 레이아웃 계산 오케스트레이션
// 요구사항:
//  - 루트 constraints로 전체 트리 배치
//  - 캐시 연동 및 dirty subtree 최적화
//  - 측정/배치 단계 분리
// 의존성:
//  - Runtime/Layout/ConstraintSolver
//  - Runtime/Layout/LayoutCache
//  - Runtime/Tree/WidgetTree
// 구현 단계: Phase B
// 성능 고려사항:
//  - 레이아웃 재계산 범위 최소화
//  - 깊은 트리 재귀 비용 관리
// 테스트 전략:
//  - 복합 레이아웃 회귀 테스트
//  - dirty subtree만 갱신되는지 테스트

} // namespace rex::ui::runtime::layout
