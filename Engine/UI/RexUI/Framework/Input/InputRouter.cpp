#include "InputRouter.h"

#include "../../Core/Event.h"
#include "../../Core/Widget.h"
#include "InputMap.h"

namespace rex::ui::framework::input {

InputRouter::InputRouter(InputMap* map)
    : map_(map) {}

void InputRouter::setActiveContext(const std::string& context) {
    activeContext_ = context;
}

const std::string& InputRouter::activeContext() const {
    return activeContext_;
}

bool InputRouter::routeToWidgetTree(core::Widget* root, const InputEvent& event) {
    if (!root) return false;

    core::EventContext eventCtx{};
    eventCtx.root = root;
    eventCtx.target = root;

    if (event.device == InputDeviceType::Mouse) {
        core::PointerEvent pointerEvent;
        pointerEvent.phase = core::EventPhase::Target;
        pointerEvent.button = static_cast<std::uint32_t>(event.code);
        pointerEvent.wheelDelta = event.value;

        if (event.type == InputEventType::ButtonDown) pointerEvent.type = core::EventType::PointerDown;
        else if (event.type == InputEventType::ButtonUp) pointerEvent.type = core::EventType::PointerUp;
        else pointerEvent.type = core::EventType::PointerMove;

        return root->handleEvent(eventCtx, pointerEvent);
    }

    if (event.device == InputDeviceType::Keyboard) {
        core::KeyboardEvent keyEvent;
        keyEvent.phase = core::EventPhase::Target;
        keyEvent.keyCode = event.code;
        keyEvent.type = (event.type == InputEventType::ButtonDown)
            ? core::EventType::KeyDown
            : core::EventType::KeyUp;
        return root->handleEvent(eventCtx, keyEvent);
    }

    core::TextInputEvent textEvent;
    textEvent.phase = core::EventPhase::Target;
    textEvent.type = core::EventType::TextInput;
    textEvent.text = event.text;
    return root->handleEvent(eventCtx, textEvent);
}

bool InputRouter::routeToAction(const InputEvent& event, std::string& outActionName) {
    if (!map_) return false;
    return map_->resolveAction(activeContext_, event, outActionName);
}

} // namespace rex::ui::framework::input

