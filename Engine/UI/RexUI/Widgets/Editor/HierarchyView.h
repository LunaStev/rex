#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "../../Core/Widget.h"

namespace rex::ui::widgets::editor {

struct HierarchyNode {
    std::uint64_t id = 0;
    std::string label;
    bool expanded = true;
    std::vector<HierarchyNode> children;
};

class HierarchyView : public core::Widget {
public:
    using SelectHandler = std::function<void(std::uint64_t)>;

    void setNodes(std::vector<HierarchyNode> nodes);
    void setFilterText(const std::string& filterText);
    void setOnSelect(SelectHandler handler);

    std::uint64_t selectedNode() const;
};

// TODO [RexUI-Widgets-Editor-001]:
// 책임: 에디터 계층 트리 위젯 인터페이스 선언
// 요구사항:
//  - 노드 표시/확장/선택 API
//  - 필터 텍스트 적용 훅
//  - 가상화(VirtualTree) 연동 지점
// 의존성:
//  - Core/Widget
//  - Runtime/Virtualization/VirtualTree
// 구현 단계: Phase E
// 성능 고려사항:
//  - 대규모 노드 렌더 최적화
//  - expand/collapse 비용 최소화
// 테스트 전략:
//  - 선택/확장 동작 테스트
//  - 필터 적용 회귀 테스트

} // namespace rex::ui::widgets::editor
