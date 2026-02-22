#pragma once

#include <vector>

#include "PatchOp.h"

namespace rex::ui::runtime::tree {
class WidgetTree;
}

namespace rex::ui::runtime::diff {

class DiffEngine {
public:
    std::vector<PatchOp> diff(const tree::WidgetTree& previousTree, const tree::WidgetTree& nextTree) const;
    bool apply(const std::vector<PatchOp>& patchOps, tree::WidgetTree& targetTree) const;
};

// TODO [RexUI-Runtime-Diff-002]:
// 책임: 이전/다음 WidgetTree 차이 계산 및 적용
// 요구사항:
//  - key 기반 최소 패치 생성
//  - 안전한 패치 적용(실패 롤백 전략)
//  - subtree 단위 diff 최적화
// 의존성:
//  - Runtime/Diff/PatchOp
//  - Runtime/Tree/WidgetTree
// 구현 단계: Phase D
// 성능 고려사항:
//  - O(N)~O(N log N) 목표
//  - 대규모 트리에서 allocations 제한
// 테스트 전략:
//  - diff 결과 결정성 테스트
//  - apply 후 구조 동등성 테스트

} // namespace rex::ui::runtime::diff
