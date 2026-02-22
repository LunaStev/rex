#include "LayoutEngine.h"

#include <functional>

#include "../../Core/Event.h"
#include "../Tree/WidgetTree.h"

namespace rex::ui::runtime::layout {

namespace {
void syncElementTree(tree::WidgetTree& widgetTree) {
    auto& elements = widgetTree.elements();
    elements.clear();

    std::function<void(core::Widget*, core::ElementId)> visit = [&](core::Widget* widget, core::ElementId parentId) {
        if (!widget) return;
        elements.upsert({
            widget->id(),
            parentId,
            widget->arrangedRect(),
            true,
            1
        });
        for (const auto& child : widget->children()) {
            visit(child.get(), widget->id());
        }
    };

    auto root = widgetTree.root();
    visit(root.get(), core::kInvalidElementId);
}
} // namespace

LayoutEngine::LayoutEngine(ConstraintSolver* solver, LayoutCache* cache)
    : solver_(solver)
    , cache_(cache) {}

void LayoutEngine::compute(tree::WidgetTree& tree, const core::LayoutConstraints& rootConstraints, std::uint64_t frameStamp) {
    auto root = tree.root();
    if (!root || !solver_ || !cache_) return;

    cache_->beginFrame(frameStamp);

    core::EventContext eventCtx{};
    eventCtx.root = root.get();

    LayoutCacheKey key;
    key.elementId = root->id();
    key.constraints = rootConstraints;
    key.styleHash = 0;
    key.contentHash = 0;

    core::LayoutResult result;
    const auto cached = cache_->find(key);
    if (cached.has_value()) {
        result = *cached;
    } else {
        const core::Size desired = root->measure(rootConstraints, eventCtx);
        result.desired = solver_->solve(rootConstraints, desired);
        result.arranged = {
            0.0f,
            0.0f,
            result.desired.w,
            result.desired.h
        };
        cache_->store({key, result, frameStamp});
    }

    root->arrange(result.arranged);
    tree.rebuildIndex();
    syncElementTree(tree);
}

} // namespace rex::ui::runtime::layout

