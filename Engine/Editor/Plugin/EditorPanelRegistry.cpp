#include "EditorPanelRegistry.h"

namespace rex::editor::plugin {

bool EditorPanelRegistry::registerPanel(const std::string& panelId, Factory factory) {
    if (panelId.empty() || !factory) return false;
    return factories_.emplace(panelId, std::move(factory)).second;
}

void EditorPanelRegistry::unregisterPanel(const std::string& panelId) {
    factories_.erase(panelId);
}

std::shared_ptr<panels::IEditorPanel> EditorPanelRegistry::create(const std::string& panelId) const {
    const auto it = factories_.find(panelId);
    if (it == factories_.end()) return {};
    return it->second ? it->second() : nullptr;
}

bool EditorPanelRegistry::contains(const std::string& panelId) const {
    return factories_.find(panelId) != factories_.end();
}

} // namespace rex::editor::plugin

