#include "InputMap.h"

#include <algorithm>

namespace rex::ui::framework::input {

namespace {
bool isSameTrigger(const InputEvent& a, const InputEvent& b) {
    return a.device == b.device &&
           a.type == b.type &&
           a.code == b.code &&
           a.value == b.value &&
           a.text == b.text;
}
} // namespace

void InputMap::bindAction(const std::string& context, const std::string& actionName, const InputEvent& trigger) {
    auto& bindings = bindingsByContext_[context];

    auto it = std::find_if(bindings.begin(), bindings.end(), [&](const ActionBinding& b) {
        return b.actionName == actionName;
    });
    if (it != bindings.end()) {
        it->trigger = trigger;
        return;
    }

    bindings.push_back({actionName, trigger});
}

void InputMap::unbindAction(const std::string& context, const std::string& actionName) {
    auto it = bindingsByContext_.find(context);
    if (it == bindingsByContext_.end()) return;

    auto& bindings = it->second;
    bindings.erase(std::remove_if(bindings.begin(), bindings.end(), [&](const ActionBinding& b) {
        return b.actionName == actionName;
    }), bindings.end());
}

bool InputMap::resolveAction(const std::string& context, const InputEvent& event, std::string& outActionName) const {
    const auto it = bindingsByContext_.find(context);
    if (it == bindingsByContext_.end()) return false;

    for (const auto& binding : it->second) {
        if (!isSameTrigger(binding.trigger, event)) continue;
        outActionName = binding.actionName;
        return true;
    }
    return false;
}

} // namespace rex::ui::framework::input

