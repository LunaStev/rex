#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "DockNode.h"

namespace rex::ui::framework::docking {

class DockManager {
public:
    DockManager();

    DockNodeId root() const;

    DockPanelId createPanel(const std::string& panelName);
    bool destroyPanel(DockPanelId panelId);

    bool dock(DockPanelId panelId, DockNodeId targetNode, DockSplitDirection direction, float ratio);
    bool undock(DockPanelId panelId);
    bool setActiveTab(DockNodeId tabStackNode, DockPanelId panelId);

    bool split(DockNodeId node, DockSplitDirection direction, float ratio);
    bool merge(DockNodeId node);

    const DockNode* findNode(DockNodeId id) const;
    const std::unordered_map<DockNodeId, DockNode>& nodes() const;

    const std::unordered_map<DockPanelId, std::string>& panels() const;

private:
    friend class DockSerializer;

    DockNodeId createNode(DockNodeType type, DockNodeId parentId);
    void removePanelFromNode(DockNode& node, DockPanelId panelId);

    DockNodeId root_ = 0;
    DockNodeId nextNodeId_ = 1;
    DockPanelId nextPanelId_ = 1;

    std::unordered_map<DockNodeId, DockNode> nodes_;
    std::unordered_map<DockPanelId, std::string> panels_;
};

// TODO [RexUI-Framework-Docking-002]:
// 책임: 도킹/분리/탭 활성화 동작 제어
// 요구사항:
//  - 패널 생성/파괴 수명주기
//  - 노드 split/merge 연산
//  - 드래그 도킹 목표 계산 API
// 의존성:
//  - Docking/DockNode
// 구현 단계: Phase E
// 성능 고려사항:
//  - 빈번한 도킹 조작 O(logN) 목표
//  - 레이아웃 invalidation 최소화
// 테스트 전략:
//  - 복합 도킹 시퀀스 테스트
//  - 부유창(Floating) 전환 테스트

} // namespace rex::ui::framework::docking
