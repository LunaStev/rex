#include "ContentBrowserPanel.h"

#include "../Core/EditorApp.h"

namespace rex::editor::panels {

bool ContentBrowserPanel::onAttach(core::EditorApp& app) {
    if (dockPanelId_ == 0) {
        dockPanelId_ = app.dockManager().createPanel(title());
    }

    auto& store = app.stateStore().rawStore();
    store.beginBatch();
    store.set("editor.panels.content_browser.visible", true);
    store.set("editor.panels.content_browser.path_filter", pathFilter_);
    store.set("editor.panels.content_browser.tag_filter", tagFilter_);
    store.endBatch();
    return true;
}

void ContentBrowserPanel::onDetach(core::EditorApp& app) {
    if (dockPanelId_ != 0) {
        app.dockManager().destroyPanel(dockPanelId_);
        dockPanelId_ = 0;
    }

    auto& store = app.stateStore().rawStore();
    store.remove("editor.panels.content_browser.visible");
    store.remove("editor.panels.content_browser.path_filter");
    store.remove("editor.panels.content_browser.tag_filter");
    store.remove("editor.panels.content_browser.selected_asset_count");
}

void ContentBrowserPanel::onTick(core::EditorApp& app, float dt) {
    (void)dt;
    const auto& typed = app.stateStore().state();

    auto& store = app.stateStore().rawStore();
    store.beginBatch();
    store.set("editor.panels.content_browser.path_filter", pathFilter_);
    store.set("editor.panels.content_browser.tag_filter", tagFilter_);
    store.set("editor.panels.content_browser.selected_asset_count",
              static_cast<std::int64_t>(typed.selectedAssets.size()));
    store.endBatch();
}

void ContentBrowserPanel::setPathFilter(const std::string& filter) {
    pathFilter_ = filter;
}

void ContentBrowserPanel::setTagFilter(const std::string& filter) {
    tagFilter_ = filter;
}

} // namespace rex::editor::panels
