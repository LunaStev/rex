#pragma once

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
