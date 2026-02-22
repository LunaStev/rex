#include "ViewportPanel.h"

#include "../Core/EditorApp.h"

namespace rex::editor::panels {

namespace {
std::int64_t renderModeToValue(core::ViewportRenderMode mode) {
    switch (mode) {
        case core::ViewportRenderMode::Lit: return 0;
        case core::ViewportRenderMode::Wireframe: return 1;
        case core::ViewportRenderMode::LightingOnly: return 2;
    }
    return 0;
}
}

bool ViewportPanel::onAttach(core::EditorApp& app) {
    if (dockPanelId_ == 0) {
        dockPanelId_ = app.dockManager().createPanel(title());
    }

    auto& store = app.stateStore().rawStore();
    store.beginBatch();
    store.set("editor.panels.viewport.visible", true);
    store.set("editor.panels.viewport.debug_overlay", debugOverlay_);
    store.set("editor.panels.viewport.selection_outline", showSelectionOutline_);
    store.set("editor.panels.viewport.post_process", showPostProcess_);
    store.endBatch();
    return true;
}

void ViewportPanel::onDetach(core::EditorApp& app) {
    if (dockPanelId_ != 0) {
        app.dockManager().destroyPanel(dockPanelId_);
        dockPanelId_ = 0;
    }

    auto& store = app.stateStore().rawStore();
    store.remove("editor.panels.viewport.visible");
    store.remove("editor.panels.viewport.debug_overlay");
    store.remove("editor.panels.viewport.selection_outline");
    store.remove("editor.panels.viewport.post_process");
}

void ViewportPanel::onTick(core::EditorApp& app, float dt) {
    (void)dt;

    const auto& typed = app.stateStore().state();
    showPostProcess_ = typed.postProcessEnabled;

    auto& store = app.stateStore().rawStore();
    store.beginBatch();
    store.set("editor.panels.viewport.debug_overlay", debugOverlay_);
    store.set("editor.panels.viewport.selection_outline", showSelectionOutline_);
    store.set("editor.panels.viewport.post_process", showPostProcess_);
    store.set("editor.panels.viewport.render_mode", renderModeToValue(typed.viewportRenderMode));
    store.set("editor.panels.viewport.selected_entity_count",
              static_cast<std::int64_t>(typed.selectedEntities.size()));
    store.endBatch();
}

void ViewportPanel::setDebugOverlay(bool enabled) {
    debugOverlay_ = enabled;
}

bool ViewportPanel::debugOverlayEnabled() const {
    return debugOverlay_;
}

} // namespace rex::editor::panels
