#include "TextWidget.h"

#include <algorithm>
#include <cmath>

namespace rex::ui::widgets::basic {

void TextWidget::setText(const std::string& text) {
    text_ = text;
}

const std::string& TextWidget::text() const {
    return text_;
}

void TextWidget::setWrapping(bool enabled) {
    wrapping_ = enabled;
}

bool TextWidget::wrapping() const {
    return wrapping_;
}

core::Size TextWidget::measure(const core::LayoutConstraints& constraints, const core::EventContext& ctx) const {
    (void)ctx;
    constexpr float charWidth = 8.0f;
    constexpr float lineHeight = 18.0f;
    float width = static_cast<float>(text_.size()) * charWidth;
    float height = lineHeight;

    if (wrapping_ && constraints.max.w > 0.0f && width > constraints.max.w) {
        const float charsPerLine = std::max(1.0f, constraints.max.w / charWidth);
        const float lines = std::ceil(static_cast<float>(text_.size()) / charsPerLine);
        width = constraints.max.w;
        height = lineHeight * lines;
    }

    width = std::max(width, constraints.min.w);
    height = std::max(height, constraints.min.h);
    if (constraints.max.w > 0.0f) width = std::min(width, constraints.max.w);
    if (constraints.max.h > 0.0f) height = std::min(height, constraints.max.h);
    return {width, height};
}

void TextWidget::paint(core::PaintContext& ctx) const {
    ctx.drawText(arrangedRect(), text_, core::Color{1.0f, 1.0f, 1.0f, 1.0f});
    core::Widget::paint(ctx);
}

} // namespace rex::ui::widgets::basic
