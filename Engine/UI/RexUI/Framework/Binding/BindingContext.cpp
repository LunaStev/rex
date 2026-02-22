#include "BindingContext.h"

#include <charconv>
#include <string>
#include <type_traits>

#include "../../Widgets/Basic/ButtonWidget.h"
#include "../../Widgets/Basic/TextWidget.h"
#include "../State/UIStateStore.h"

namespace rex::ui::framework::binding {

namespace {
std::string stateValueToString(const state::StateValue& value) {
    return std::visit([](auto&& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            return {};
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::int64_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else {
            return {};
        }
    }, value);
}

void applyToWidget(core::Widget* widget, const BindingExpression& expression, const state::StateValue& value) {
    if (!widget) return;

    const std::string text = stateValueToString(value);
    if (expression.targetProperty == "styleClass") {
        widget->setStyleClass(text);
        return;
    }

    if (expression.targetProperty == "text") {
        if (auto* textWidget = dynamic_cast<widgets::basic::TextWidget*>(widget)) {
            textWidget->setText(text);
            return;
        }
        if (auto* buttonWidget = dynamic_cast<widgets::basic::ButtonWidget*>(widget)) {
            buttonWidget->setText(text);
            return;
        }
    }
}
} // namespace

BindingContext::BindingContext(state::UIStateStore* stateStore, core::Widget* rootWidget)
    : stateStore_(stateStore)
    , rootWidget_(rootWidget) {}

state::UIStateStore* BindingContext::stateStore() const {
    return stateStore_;
}

core::Widget* BindingContext::rootWidget() const {
    return rootWidget_;
}

BindingEngine::BindingId BindingEngine::bind(core::Widget* widget, const BindingExpression& expression, BindingContext& context) {
    if (!widget || !context.stateStore()) return 0;

    const BindingId id = nextBindingId_++;
    BindingRecord record{};
    record.widget = widget;
    record.expression = expression;

    record.stateSubscriptionId = context.stateStore()->subscribe(
        expression.sourcePath,
        [this, id](const state::StatePath& changedPath, const state::StateValue& value) {
            auto it = bindings_.find(id);
            if (it == bindings_.end()) return;
            if (changedPath != it->second.expression.sourcePath) return;
            applyToWidget(it->second.widget, it->second.expression, value);
        });

    bindings_[id] = record;

    const auto current = context.stateStore()->get(expression.sourcePath);
    if (current.has_value()) {
        applyToWidget(widget, expression, *current);
    }

    return id;
}

void BindingEngine::unbind(BindingId id, BindingContext& context) {
    auto it = bindings_.find(id);
    if (it == bindings_.end()) return;
    if (context.stateStore() && it->second.stateSubscriptionId != 0) {
        context.stateStore()->unsubscribe(it->second.stateSubscriptionId);
    }
    bindings_.erase(it);
}

void BindingEngine::evaluate(BindingContext& context) {
    if (!context.stateStore()) return;
    for (const auto& [id, record] : bindings_) {
        (void)id;
        if (!record.widget) continue;
        const auto current = context.stateStore()->get(record.expression.sourcePath);
        if (!current.has_value()) continue;
        applyToWidget(record.widget, record.expression, *current);
    }
}

void BindingEngine::flush(BindingContext& context) {
    evaluate(context);
}

} // namespace rex::ui::framework::binding
