#include "ButtonWidget.h"

#include <algorithm>
#include <utility>

namespace rex::ui::widgets::basic {

void ButtonWidget::setText(const std::string& text) {
    text_ = text;
}

const std::string& ButtonWidget::text() const {
    return text_;
}

void ButtonWidget::setOnClick(ClickHandler handler) {
    onClick_ = std::move(handler);
}

core::Size ButtonWidget::measure(const core::LayoutConstraints& constraints, const core::EventContext& ctx) const {
    (void)ctx;
    constexpr float charWidth = 8.0f;
    constexpr float paddingX = 20.0f;
    constexpr float minHeight = 26.0f;

    float width = std::max(constraints.min.w, static_cast<float>(text_.size()) * charWidth + paddingX);
    float height = std::max(constraints.min.h, minHeight);

    if (constraints.max.w > 0.0f) width = std::min(width, constraints.max.w);
    if (constraints.max.h > 0.0f) height = std::min(height, constraints.max.h);
    return {width, height};
}

void ButtonWidget::paint(core::PaintContext& ctx) const {
    const core::Color bg = pressed_
        ? core::Color{0.25f, 0.25f, 0.30f, 1.0f}
        : core::Color{0.17f, 0.17f, 0.20f, 1.0f};
    ctx.drawRect(arrangedRect(), bg);
    ctx.drawBorder(arrangedRect(), core::Color{0.10f, 0.10f, 0.10f, 1.0f}, 1.0f);
    ctx.drawText(arrangedRect(), text_, core::Color{0.95f, 0.95f, 0.95f, 1.0f});
    core::Widget::paint(ctx);
}

bool ButtonWidget::handleEvent(core::EventContext& ctx, core::RoutedEvent& event) {
    (void)ctx;
    if (event.type == core::EventType::PointerDown) {
        pressed_ = true;
        event.handled = true;
        return true;
    }
    if (event.type == core::EventType::PointerUp) {
        const bool wasPressed = pressed_;
        pressed_ = false;
        if (wasPressed && onClick_) {
            onClick_();
        }
        event.handled = wasPressed;
        return wasPressed;
    }
    return core::Widget::handleEvent(ctx, event);
}

} // namespace rex::ui::widgets::basic
