#include "EditorModuleManager.h"

namespace rex::editor::plugin {

bool EditorModuleManager::load(const std::shared_ptr<IEditorModule>& module) {
    if (!module) return false;
    const std::string moduleName = module->name() ? module->name() : "";
    if (moduleName.empty()) return false;
    if (loaded_.find(moduleName) != loaded_.end()) return false;

    if (!module->startup(panels_, tools_, inspectors_, commands_, shortcuts_)) {
        return false;
    }

    loaded_[moduleName] = module;
    return true;
}

bool EditorModuleManager::unload(const std::string& moduleName) {
    const auto it = loaded_.find(moduleName);
    if (it == loaded_.end()) return false;
    it->second->shutdown(panels_, tools_, inspectors_, commands_, shortcuts_);
    loaded_.erase(it);
    return true;
}

bool EditorModuleManager::isLoaded(const std::string& moduleName) const {
    return loaded_.find(moduleName) != loaded_.end();
}

EditorPanelRegistry& EditorModuleManager::panelRegistry() {
    return panels_;
}

EditorToolRegistry& EditorModuleManager::toolRegistry() {
    return tools_;
}

InspectorWidgetRegistry& EditorModuleManager::inspectorRegistry() {
    return inspectors_;
}

CommandRegistry& EditorModuleManager::commandRegistry() {
    return commands_;
}

ShortcutMap& EditorModuleManager::shortcutMap() {
    return shortcuts_;
}

} // namespace rex::editor::plugin

