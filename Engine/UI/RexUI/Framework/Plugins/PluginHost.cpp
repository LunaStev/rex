#include "PluginHost.h"

namespace rex::ui::framework::plugins {

bool PluginHost::load(const std::shared_ptr<IUIPlugin>& plugin) {
    if (!plugin) return false;
    const std::string pluginName = plugin->name() ? plugin->name() : "";
    if (pluginName.empty()) return false;
    if (loadedPlugins_.find(pluginName) != loadedPlugins_.end()) return false;

    if (!plugin->onLoad(widgetRegistry_, panelRegistry_)) return false;
    loadedPlugins_[pluginName] = plugin;
    return true;
}

bool PluginHost::unload(const std::string& pluginName) {
    const auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end()) return false;
    it->second->onUnload(widgetRegistry_, panelRegistry_);
    loadedPlugins_.erase(it);
    return true;
}

bool PluginHost::isLoaded(const std::string& pluginName) const {
    return loadedPlugins_.find(pluginName) != loadedPlugins_.end();
}

WidgetRegistry& PluginHost::widgetRegistry() {
    return widgetRegistry_;
}

PanelRegistry& PluginHost::panelRegistry() {
    return panelRegistry_;
}

} // namespace rex::ui::framework::plugins

