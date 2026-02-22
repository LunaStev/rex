#include "DetailsPanel.h"

#include "../Core/EditorApp.h"

namespace rex::editor::panels {

bool DetailsPanel::onAttach(core::EditorApp& app) {
    if (dockPanelId_ == 0) {
        dockPanelId_ = app.dockManager().createPanel(title());
    }

    auto& store = app.stateStore().rawStore();
    store.beginBatch();
    store.set("editor.panels.details.visible", true);
    store.set("editor.panels.details.show_advanced", showAdvanced_);
    store.endBatch();
    return true;
}

void DetailsPanel::onDetach(core::EditorApp& app) {
    if (dockPanelId_ != 0) {
        app.dockManager().destroyPanel(dockPanelId_);
        dockPanelId_ = 0;
    }

    auto& store = app.stateStore().rawStore();
    store.remove("editor.panels.details.visible");
    store.remove("editor.panels.details.show_advanced");
    store.remove("editor.panels.details.has_selection");
    store.remove("editor.panels.details.selection_count");
}

void DetailsPanel::onTick(core::EditorApp& app, float dt) {
    (void)dt;
    const auto& typed = app.stateStore().state();

    auto& store = app.stateStore().rawStore();
    store.beginBatch();
    store.set("editor.panels.details.show_advanced", showAdvanced_);
    store.set("editor.panels.details.selection_count",
              static_cast<std::int64_t>(typed.selectedEntities.size()));
    store.set("editor.panels.details.has_selection", !typed.selectedEntities.empty());
    store.set("editor.panels.details.can_undo", app.commandBus().canUndo());
    store.set("editor.panels.details.can_redo", app.commandBus().canRedo());
    store.endBatch();
}

void DetailsPanel::setShowAdvanced(bool enabled) {
    showAdvanced_ = enabled;
}

bool DetailsPanel::showAdvanced() const {
    return showAdvanced_;
}

} // namespace rex::editor::panels
