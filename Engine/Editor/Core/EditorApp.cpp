#include "EditorApp.h"

#include <array>
#include <memory>

#include "../Panels/ContentBrowserPanel.h"
#include "../Panels/DetailsPanel.h"
#include "../Panels/OutlinerPanel.h"
#include "../Panels/ViewportPanel.h"

namespace rex::editor::core {

namespace {
bool registerPanelFactory(plugin::EditorPanelRegistry& registry,
                          const std::string& panelId,
                          plugin::EditorPanelRegistry::Factory factory) {
    if (registry.contains(panelId)) return true;
    return registry.registerPanel(panelId, std::move(factory));
}
}

EditorApp::EditorApp(ui::app::RexUIEngine* uiEngine)
    : uiEngine_(uiEngine) {}

bool EditorApp::initialize(const EditorWorkspace& workspace) {
    if (!uiEngine_) return false;
    if (!session_.open(workspace)) return false;

    stateStore_.state().projectPath = workspace.projectRoot;
    if (!registerCorePanels()) {
        session_.close();
        return false;
    }
    if (!attachCorePanels()) {
        session_.close();
        return false;
    }

    publishBootstrapState();
    return true;
}

void EditorApp::shutdown() {
    if (!session_.isOpen()) return;
    detachPanels();
    session_.close();
}

bool EditorApp::tick(float dt, std::uint64_t frameIndex) {
    if (!session_.isOpen() || !uiEngine_) return false;

    syncStateFromManagers();
    for (const auto& panel : activePanels_) {
        if (!panel) continue;
        panel->onTick(*this, dt);
    }

    return uiEngine_->runFrame(dt, frameIndex);
}

EditorStateStore& EditorApp::stateStore() {
    return stateStore_;
}

EditorCommandBus& EditorApp::commandBus() {
    return commandBus_;
}

SelectionManager& EditorApp::selection() {
    return selection_;
}

TransactionSystem& EditorApp::transactions() {
    return transactions_;
}

ui::framework::docking::DockManager& EditorApp::dockManager() {
    return dockManager_;
}

plugin::EditorModuleManager& EditorApp::moduleManager() {
    return moduleManager_;
}

const std::vector<std::shared_ptr<panels::IEditorPanel>>& EditorApp::activePanels() const {
    return activePanels_;
}

bool EditorApp::registerCorePanels() {
    auto& registry = moduleManager_.panelRegistry();

    bool ok = true;
    ok = ok && registerPanelFactory(registry, "viewport", []() {
        return std::make_shared<panels::ViewportPanel>();
    });
    ok = ok && registerPanelFactory(registry, "outliner", []() {
        return std::make_shared<panels::OutlinerPanel>();
    });
    ok = ok && registerPanelFactory(registry, "details", []() {
        return std::make_shared<panels::DetailsPanel>();
    });
    ok = ok && registerPanelFactory(registry, "content_browser", []() {
        return std::make_shared<panels::ContentBrowserPanel>();
    });
    return ok;
}

bool EditorApp::attachCorePanels() {
    static constexpr std::array<const char*, 4> kCorePanelIds = {
        "viewport",
        "outliner",
        "details",
        "content_browser",
    };

    detachPanels();
    activePanels_.reserve(kCorePanelIds.size());

    auto& registry = moduleManager_.panelRegistry();
    for (const char* panelId : kCorePanelIds) {
        auto panel = registry.create(panelId);
        if (!panel) {
            detachPanels();
            return false;
        }
        if (!panel->onAttach(*this)) {
            detachPanels();
            return false;
        }
        activePanels_.push_back(std::move(panel));
    }

    stateStore_.rawStore().set("editor.panels.active_count",
        static_cast<std::int64_t>(activePanels_.size()));
    return true;
}

void EditorApp::detachPanels() {
    for (auto& panel : activePanels_) {
        if (!panel) continue;
        panel->onDetach(*this);
    }
    activePanels_.clear();
    stateStore_.rawStore().set("editor.panels.active_count", static_cast<std::int64_t>(0));
}

void EditorApp::syncStateFromManagers() {
    auto& typed = stateStore_.state();
    typed.selectedEntities.clear();
    for (const auto id : selection_.selectedEntities()) {
        typed.selectedEntities.insert(id);
    }

    typed.selectedAssets.clear();
    for (const auto& asset : selection_.selectedAssets()) {
        typed.selectedAssets.insert(asset);
    }

    auto& store = stateStore_.rawStore();
    store.beginBatch();
    store.set("editor.selection.entity_count",
        static_cast<std::int64_t>(typed.selectedEntities.size()));
    store.set("editor.selection.asset_count",
        static_cast<std::int64_t>(typed.selectedAssets.size()));
    store.set("editor.commands.can_undo", commandBus_.canUndo());
    store.set("editor.commands.can_redo", commandBus_.canRedo());
    store.set("editor.transactions.open", transactions_.inTransaction());
    store.set("editor.session.id", static_cast<std::int64_t>(session_.sessionId()));
    store.endBatch();
}

void EditorApp::publishBootstrapState() {
    const auto& typed = stateStore_.state();
    auto& store = stateStore_.rawStore();
    store.beginBatch();
    store.set("editor.project.path", typed.projectPath);
    store.set("editor.scene.path", typed.activeScenePath);
    store.set("editor.scene.dirty", typed.sceneDirty);
    store.set("editor.mode", static_cast<std::int64_t>(typed.mode));
    store.set("editor.viewport.render_mode", static_cast<std::int64_t>(typed.viewportRenderMode));
    store.set("editor.viewport.post_process", typed.postProcessEnabled);
    store.endBatch();
}

} // namespace rex::editor::core
