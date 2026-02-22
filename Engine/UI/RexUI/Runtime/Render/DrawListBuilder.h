#pragma once

#include "DrawCommand.h"

namespace rex::ui::runtime::tree {
class WidgetTree;
}

namespace rex::ui::runtime::render {

class DrawListBuilder {
public:
    DrawList build(const tree::WidgetTree& tree) const;
};

// TODO [RexUI-Runtime-Render-002]:
// 책임: WidgetTree -> DrawList 변환
// 요구사항:
//  - paint 순서 보장
//  - clip stack 반영
//  - 상태 변경 최소화 순서화 옵션
// 의존성:
//  - Runtime/Tree/WidgetTree
//  - Runtime/Render/DrawCommand
// 구현 단계: Phase D
// 성능 고려사항:
//  - draw list 생성 중 임시 할당 최소화
//  - command culling 지원
// 테스트 전략:
//  - 그리기 순서 회귀 테스트
//  - clip 적용 정확성 테스트

} // namespace rex::ui::runtime::render
