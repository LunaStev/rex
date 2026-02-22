#include "EditorToolRegistry.h"

namespace rex::editor::plugin {

bool EditorToolRegistry::registerTool(const std::string& toolId, Factory factory) {
    if (toolId.empty() || !factory) return false;
    return factories_.emplace(toolId, std::move(factory)).second;
}

void EditorToolRegistry::unregisterTool(const std::string& toolId) {
    factories_.erase(toolId);
}

std::shared_ptr<tools::IEditorTool> EditorToolRegistry::create(const std::string& toolId) const {
    const auto it = factories_.find(toolId);
    if (it == factories_.end()) return {};
    return it->second ? it->second() : nullptr;
}

bool EditorToolRegistry::contains(const std::string& toolId) const {
    return factories_.find(toolId) != factories_.end();
}

} // namespace rex::editor::plugin

