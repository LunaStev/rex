#include "PanelRegistry.h"

namespace rex::ui::framework::plugins {

bool PanelRegistry::registerPanel(const PanelDescriptor& descriptor) {
    if (descriptor.id.empty()) return false;
    return panels_.emplace(descriptor.id, descriptor).second;
}

void PanelRegistry::unregisterPanel(const std::string& panelId) {
    panels_.erase(panelId);
}

bool PanelRegistry::hasPanel(const std::string& panelId) const {
    return panels_.find(panelId) != panels_.end();
}

PanelDescriptor PanelRegistry::getPanel(const std::string& panelId) const {
    const auto it = panels_.find(panelId);
    if (it == panels_.end()) return {};
    return it->second;
}

} // namespace rex::ui::framework::plugins

