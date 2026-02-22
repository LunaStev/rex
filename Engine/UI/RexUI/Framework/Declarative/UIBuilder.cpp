#include "UIBuilder.h"

#include <any>
#include <memory>
#include <string>

#include "../../Widgets/Basic/ButtonWidget.h"
#include "../../Widgets/Basic/PanelWidget.h"
#include "../../Widgets/Basic/TextWidget.h"

namespace rex::ui::framework::declarative {

namespace {
using WidgetPtr = std::shared_ptr<core::Widget>;

WidgetPtr createWidgetByType(const std::string& type) {
    if (type == "Panel") return std::make_shared<widgets::basic::PanelWidget>();
    if (type == "Text") return std::make_shared<widgets::basic::TextWidget>();
    if (type == "Button") return std::make_shared<widgets::basic::ButtonWidget>();
    return std::make_shared<widgets::basic::PanelWidget>();
}

void applyProps(const ViewNodeDesc& desc, core::Widget* widget) {
    if (!widget) return;

    const auto styleIt = desc.props.find("styleClass");
    if (styleIt != desc.props.end() && styleIt->second.type() == typeid(std::string)) {
        widget->setStyleClass(std::any_cast<std::string>(styleIt->second));
    }

    if (auto* textWidget = dynamic_cast<widgets::basic::TextWidget*>(widget)) {
        const auto textIt = desc.props.find("text");
        if (textIt != desc.props.end() && textIt->second.type() == typeid(std::string)) {
            textWidget->setText(std::any_cast<std::string>(textIt->second));
        }
    }

    if (auto* buttonWidget = dynamic_cast<widgets::basic::ButtonWidget*>(widget)) {
        const auto textIt = desc.props.find("text");
        if (textIt != desc.props.end() && textIt->second.type() == typeid(std::string)) {
            buttonWidget->setText(std::any_cast<std::string>(textIt->second));
        }
    }
}

WidgetPtr buildRecursive(const ViewNodeDesc& desc) {
    auto widget = createWidgetByType(desc.type);
    applyProps(desc, widget.get());

    for (const auto& childDesc : desc.children) {
        widget->addChild(buildRecursive(childDesc));
    }

    return widget;
}
} // namespace

UIBuilder::WidgetPtr UIBuilder::build(const ViewNodeDesc& rootDesc) const {
    return buildRecursive(rootDesc);
}

UIBuilder::WidgetPtr UIBuilder::patch(const WidgetPtr& currentRoot, const ViewNodeDesc& nextDesc) const {
    (void)currentRoot;
    return build(nextDesc);
}

} // namespace rex::ui::framework::declarative

