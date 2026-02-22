#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "../../Core/Widget.h"

namespace rex::ui::widgets::editor {

struct DockTabItem {
    std::uint64_t panelId = 0;
    std::string title;
    bool closable = true;
};

class DockTabBar : public core::Widget {
public:
    using ActivateHandler = std::function<void(std::uint64_t)>;
    using CloseHandler = std::function<void(std::uint64_t)>;

    void setTabs(std::vector<DockTabItem> tabs);
    void setActiveTab(std::uint64_t panelId);

    void setOnActivate(ActivateHandler handler);
    void setOnClose(CloseHandler handler);
};

// TODO [RexUI-Widgets-Editor-003]:
// 책임: 도킹 탭 바 인터페이스 선언
// 요구사항:
//  - 탭 활성/닫기 이벤트
//  - 드래그 시작 훅(도킹 연동)
//  - 탭 overflow 처리 정책
// 의존성:
//  - Core/Widget
//  - Framework/Docking/DockManager
// 구현 단계: Phase E
// 성능 고려사항:
//  - 탭 다수일 때 레이아웃 계산 최적화
//  - 드래그 프리뷰 비용 최소화
// 테스트 전략:
//  - 활성/닫기 동작 테스트
//  - overflow/스크롤 탭 테스트

} // namespace rex::ui::widgets::editor
