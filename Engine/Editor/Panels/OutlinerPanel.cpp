#include "OutlinerPanel.h"

#include "../Core/EditorApp.h"

namespace rex::editor::panels {

bool OutlinerPanel::onAttach(core::EditorApp& app) {
    if (dockPanelId_ == 0) {
        dockPanelId_ = app.dockManager().createPanel(title());
    }

    auto& store = app.stateStore().rawStore();
    store.beginBatch();
    store.set("editor.panels.outliner.visible", true);
    store.set("editor.panels.outliner.filter", searchFilter_);
    store.endBatch();
    return true;
}

void OutlinerPanel::onDetach(core::EditorApp& app) {
    if (dockPanelId_ != 0) {
        app.dockManager().destroyPanel(dockPanelId_);
        dockPanelId_ = 0;
    }

    auto& store = app.stateStore().rawStore();
    store.remove("editor.panels.outliner.visible");
    store.remove("editor.panels.outliner.filter");
    store.remove("editor.panels.outliner.selected_entity_count");
}

void OutlinerPanel::onTick(core::EditorApp& app, float dt) {
    (void)dt;
    const auto& typed = app.stateStore().state();

    auto& store = app.stateStore().rawStore();
    store.beginBatch();
    store.set("editor.panels.outliner.filter", searchFilter_);
    store.set("editor.panels.outliner.selected_entity_count",
              static_cast<std::int64_t>(typed.selectedEntities.size()));
    store.endBatch();
}

void OutlinerPanel::setSearchFilter(const std::string& filter) {
    searchFilter_ = filter;
}

const std::string& OutlinerPanel::searchFilter() const {
    return searchFilter_;
}

} // namespace rex::editor::panels
