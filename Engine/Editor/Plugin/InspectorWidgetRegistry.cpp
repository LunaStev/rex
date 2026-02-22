#include "InspectorWidgetRegistry.h"

namespace rex::editor::plugin {

bool InspectorWidgetRegistry::registerInspectorWidget(const std::string& propertyType, Factory factory) {
    if (propertyType.empty() || !factory) return false;
    factories_[propertyType] = std::move(factory);
    return true;
}

void InspectorWidgetRegistry::unregisterInspectorWidget(const std::string& propertyType) {
    factories_.erase(propertyType);
}

InspectorWidgetRegistry::Factory InspectorWidgetRegistry::find(const std::string& propertyType) const {
    const auto it = factories_.find(propertyType);
    if (it == factories_.end()) return {};
    return it->second;
}

bool InspectorWidgetRegistry::contains(const std::string& propertyType) const {
    return factories_.find(propertyType) != factories_.end();
}

} // namespace rex::editor::plugin

