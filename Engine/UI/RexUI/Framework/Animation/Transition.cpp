#include "Transition.h"

#include <algorithm>

namespace rex::ui::framework::animation {

void TransitionController::setTransition(const std::string& widgetType, const TransitionSpec& spec) {
    transitionSpecs_[widgetType].push_back(spec);
}

bool TransitionController::start(const std::string& widgetType, const std::string& from, const std::string& to) {
    const auto it = transitionSpecs_.find(widgetType);
    if (it == transitionSpecs_.end()) return false;

    for (const auto& spec : it->second) {
        if (spec.stateFrom != from || spec.stateTo != to) continue;
        activeTransitions_.push_back({widgetType, spec, 0.0f});
        return true;
    }
    return false;
}

void TransitionController::tick(float dt) {
    const float safeDt = std::max(0.0f, dt);
    for (auto& transition : activeTransitions_) {
        transition.elapsed += safeDt;
    }

    activeTransitions_.erase(
        std::remove_if(activeTransitions_.begin(), activeTransitions_.end(), [](const ActiveTransition& t) {
            return t.spec.duration <= 0.0f || t.elapsed >= t.spec.duration;
        }),
        activeTransitions_.end());
}

} // namespace rex::ui::framework::animation

