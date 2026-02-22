#pragma once

#include <unordered_map>
#include <memory>

#include "../../Core/ElementId.h"
#include "../../Core/Widget.h"
#include "ElementTree.h"

namespace rex::ui::runtime::tree {

class WidgetTree {
public:
    using WidgetPtr = std::shared_ptr<core::Widget>;

    void setRoot(const WidgetPtr& root);
    WidgetPtr root() const;

    ElementTree& elements();
    const ElementTree& elements() const;

    void rebuildIndex();
    core::Widget* findWidget(core::ElementId id) const;

private:
    WidgetPtr root_;
    ElementTree elementTree_;
    core::ElementIdGenerator idGenerator_{};
    std::unordered_map<core::ElementId, core::Widget*> widgetIndex_;
};

// TODO [RexUI-Runtime-Tree-002]:
// 책임: Widget 트리와 Element 인덱스의 동기화
// 요구사항:
//  - root 설정/조회
//  - id 기반 widget 조회 인덱스
//  - 트리 변경 시 인덱스 재구축
// 의존성:
//  - Core/Widget
//  - Runtime/Tree/ElementTree
// 구현 단계: Phase A
// 성능 고려사항:
//  - 재구축 증분화(incremental) 가능성
//  - findWidget 경로 캐시
// 테스트 전략:
//  - 깊은 트리 조회 테스트
//  - 재부모화(reparent) 무결성 테스트

} // namespace rex::ui::runtime::tree
