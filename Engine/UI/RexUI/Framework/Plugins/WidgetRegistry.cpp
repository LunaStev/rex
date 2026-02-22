#include "WidgetRegistry.h"

namespace rex::ui::framework::plugins {

bool WidgetRegistry::registerFactory(const std::string& widgetType, Factory factory) {
    if (widgetType.empty() || !factory) return false;
    return factories_.emplace(widgetType, std::move(factory)).second;
}

void WidgetRegistry::unregisterFactory(const std::string& widgetType) {
    factories_.erase(widgetType);
}

std::shared_ptr<core::Widget> WidgetRegistry::create(const std::string& widgetType) const {
    const auto it = factories_.find(widgetType);
    if (it == factories_.end()) return {};
    return it->second ? it->second() : nullptr;
}

bool WidgetRegistry::contains(const std::string& widgetType) const {
    return factories_.find(widgetType) != factories_.end();
}

} // namespace rex::ui::framework::plugins

