#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace rex::ui::framework::docking {

using DockNodeId = std::uint64_t;
using DockPanelId = std::uint64_t;

enum class DockSplitDirection {
    Horizontal,
    Vertical
};

enum class DockNodeType {
    Split,
    TabStack,
    Floating
};

struct DockNode {
    DockNodeId id = 0;
    DockNodeType type = DockNodeType::TabStack;
    DockSplitDirection splitDirection = DockSplitDirection::Horizontal;
    float splitRatio = 0.5f;

    DockNodeId parent = 0;
    DockNodeId firstChild = 0;
    DockNodeId secondChild = 0;

    std::vector<DockPanelId> tabs;
    DockPanelId activeTab = 0;
};

// TODO [RexUI-Framework-Docking-001]:
// 책임: 도킹 레이아웃 트리 노드 모델 정의
// 요구사항:
//  - Split/TabStack/Floating 노드 타입
//  - 부모/자식/활성 탭 무결성 규약
//  - 분할 비율 유효범위 검증
// 의존성:
//  - 없음
// 구현 단계: Phase E
// 성능 고려사항:
//  - 노드 탐색 인덱스 최적화
//  - 탭 이동 시 재배치 비용 최소화
// 테스트 전략:
//  - 노드 트리 무결성 테스트
//  - 분할/합치기 시나리오 테스트

} // namespace rex::ui::framework::docking
