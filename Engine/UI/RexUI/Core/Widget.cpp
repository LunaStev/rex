#include "Widget.h"

#include <algorithm>

namespace rex::ui::core {

namespace {
inline float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

Size clampSize(const LayoutConstraints& constraints, const Size& desired) {
    float minW = constraints.min.w;
    float minH = constraints.min.h;
    float maxW = constraints.max.w;
    float maxH = constraints.max.h;

    if (maxW > 0.0f && maxW < minW) std::swap(maxW, minW);
    if (maxH > 0.0f && maxH < minH) std::swap(maxH, minH);

    if (maxW <= 0.0f) maxW = desired.w < minW ? minW : desired.w;
    if (maxH <= 0.0f) maxH = desired.h < minH ? minH : desired.h;

    return {
        clampf(desired.w, minW, maxW),
        clampf(desired.h, minH, maxH)
    };
}
} // namespace

ElementId Widget::id() const {
    return id_;
}

void Widget::setId(ElementId id) {
    id_ = id;
}

Widget* Widget::parent() const {
    return parent_;
}

const std::vector<std::shared_ptr<Widget>>& Widget::children() const {
    return children_;
}

void Widget::addChild(const std::shared_ptr<Widget>& child) {
    if (!child) return;
    if (std::find(children_.begin(), children_.end(), child) != children_.end()) return;
    child->parent_ = this;
    children_.push_back(child);
}

void Widget::removeChild(ElementId childId) {
    auto it = std::find_if(children_.begin(), children_.end(), [&](const std::shared_ptr<Widget>& c) {
        return c && c->id() == childId;
    });
    if (it == children_.end()) return;
    if (*it) {
        (*it)->parent_ = nullptr;
    }
    children_.erase(it);
}

void Widget::clearChildren() {
    for (auto& child : children_) {
        if (child) {
            child->parent_ = nullptr;
        }
    }
    children_.clear();
}

Size Widget::measure(const LayoutConstraints& constraints, const EventContext& ctx) const {
    (void)ctx;

    if (children_.empty()) {
        return clampSize(constraints, constraints.min);
    }

    Size desired = constraints.min;
    for (const auto& child : children_) {
        if (!child) continue;
        const Size childSize = child->measure(constraints, ctx);
        desired.w = std::max(desired.w, childSize.w);
        desired.h = std::max(desired.h, childSize.h);
    }
    return clampSize(constraints, desired);
}

void Widget::arrange(const Rect& finalRect) {
    arrangedRect_ = finalRect;
    for (auto& child : children_) {
        if (!child) continue;
        child->arrange(finalRect);
    }
}

void Widget::paint(PaintContext& ctx) const {
    for (const auto& child : children_) {
        if (!child) continue;
        child->paint(ctx);
    }
}

bool Widget::handleEvent(EventContext& ctx, RoutedEvent& event) {
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        if (!(*it)) continue;
        if ((*it)->handleEvent(ctx, event)) {
            event.handled = true;
        }
        if (event.stopImmediate || event.stopPropagation) {
            break;
        }
    }
    return event.handled;
}

const Rect& Widget::arrangedRect() const {
    return arrangedRect_;
}

void Widget::setStyleClass(const std::string& styleClass) {
    styleClass_ = styleClass;
}

const std::string& Widget::styleClass() const {
    return styleClass_;
}

void Widget::resolveStyle(const StyleResolver& resolver, WidgetStateFlag state) {
    static constexpr const char* kDefaultTokens[] = {
        "color.text",
        "color.background",
        "color.border",
        "color.accent",
        "size.font",
        "size.border",
        "spacing.padding",
        "spacing.margin"
    };

    resolvedStyleValues_.clear();
    if (!styleClass_.empty()) {
        for (const char* token : kDefaultTokens) {
            const auto resolved = resolver.resolve(styleClass_, state, token);
            if (resolved.has_value()) {
                resolvedStyleValues_[token] = *resolved;
            }
        }
    }
    onStyleResolved();
}

std::optional<StyleValue> Widget::styleValue(const std::string& token) const {
    const auto it = resolvedStyleValues_.find(token);
    if (it == resolvedStyleValues_.end()) {
        return std::nullopt;
    }
    return it->second;
}

void Widget::onStyleResolved() {}

} // namespace rex::ui::core

