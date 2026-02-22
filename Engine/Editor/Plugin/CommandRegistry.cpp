#include "CommandRegistry.h"

namespace rex::editor::plugin {

bool CommandRegistry::registerCommand(const std::string& commandId, Factory factory) {
    if (commandId.empty() || !factory) return false;
    return factories_.emplace(commandId, std::move(factory)).second;
}

void CommandRegistry::unregisterCommand(const std::string& commandId) {
    factories_.erase(commandId);
}

std::shared_ptr<ui::framework::commands::UICommand> CommandRegistry::create(const std::string& commandId) const {
    const auto it = factories_.find(commandId);
    if (it == factories_.end()) return {};
    return it->second ? it->second() : nullptr;
}

bool CommandRegistry::contains(const std::string& commandId) const {
    return factories_.find(commandId) != factories_.end();
}

} // namespace rex::editor::plugin

