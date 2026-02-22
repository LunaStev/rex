#include "DockTabBar.h"

#include <algorithm>
#include <utility>

namespace rex::ui::widgets::editor {

void DockTabBar::setTabs(std::vector<DockTabItem> tabs) {
    tabs_ = std::move(tabs);
    if (activeTab_ == 0 && !tabs_.empty()) {
        activeTab_ = tabs_.front().panelId;
    }
}

void DockTabBar::setActiveTab(std::uint64_t panelId) {
    auto it = std::find_if(tabs_.begin(), tabs_.end(), [&](const DockTabItem& item) {
        return item.panelId == panelId;
    });
    if (it == tabs_.end()) return;
    activeTab_ = panelId;
    if (onActivate_) onActivate_(panelId);
}

void DockTabBar::setOnActivate(ActivateHandler handler) {
    onActivate_ = std::move(handler);
}

void DockTabBar::setOnClose(CloseHandler handler) {
    onClose_ = std::move(handler);
}

} // namespace rex::ui::widgets::editor

