#include "RexUIEngine.h"

#include <algorithm>

namespace rex::ui::app {

RexUIEngine::RexUIEngine(renderer::IRenderBackend* backend)
    : backend_(backend) {}

void RexUIEngine::setRoot(const std::shared_ptr<core::Widget>& root) {
    widgetTree_.setRoot(root);
    bindingContext_ = framework::binding::BindingContext(&stateStore_, root.get());
}

std::shared_ptr<core::Widget> RexUIEngine::root() const {
    return widgetTree_.root();
}

void RexUIEngine::setViewport(std::uint32_t width, std::uint32_t height) {
    viewportWidth_ = width;
    viewportHeight_ = height;
}

framework::state::UIStateStore& RexUIEngine::stateStore() {
    return stateStore_;
}

framework::binding::BindingEngine& RexUIEngine::bindingEngine() {
    return bindingEngine_;
}

framework::binding::BindingContext& RexUIEngine::bindingContext() {
    return bindingContext_;
}

runtime::tree::WidgetTree& RexUIEngine::widgetTree() {
    return widgetTree_;
}

const runtime::tree::WidgetTree& RexUIEngine::widgetTree() const {
    return widgetTree_;
}

bool RexUIEngine::runFrame(float dt, std::uint64_t frameIndex) {
    (void)dt;
    if (!backend_) return false;

    auto rootWidget = widgetTree_.root();
    if (!rootWidget) return false;

    bindingEngine_.evaluate(bindingContext_);

    const core::LayoutConstraints constraints{
        {0.0f, 0.0f},
        {static_cast<float>(viewportWidth_), static_cast<float>(viewportHeight_)}
    };
    layoutEngine_.compute(widgetTree_, constraints, frameIndex);

    const auto drawList = drawBuilder_.build(widgetTree_);
    renderGraph_.beginFrame();
    renderGraph_.addDrawList(drawList);

    const renderer::RenderFrameContext frameCtx{
        viewportWidth_,
        viewportHeight_,
        frameIndex
    };

    if (!backend_->beginFrame(frameCtx)) return false;
    if (!backend_->submit(renderGraph_.flattened())) return false;
    if (!backend_->endFrame()) return false;
    return true;
}

} // namespace rex::ui::app

