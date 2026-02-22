#include "EditorApp.h"

namespace rex::editor::core {

EditorApp::EditorApp(ui::app::RexUIEngine* uiEngine)
    : uiEngine_(uiEngine) {}

bool EditorApp::initialize(const EditorWorkspace& workspace) {
    if (!uiEngine_) return false;
    if (!session_.open(workspace)) return false;

    stateStore_.state().projectPath = workspace.projectRoot;
    return true;
}

void EditorApp::shutdown() {
    if (!session_.isOpen()) return;
    session_.close();
}

bool EditorApp::tick(float dt, std::uint64_t frameIndex) {
    if (!session_.isOpen() || !uiEngine_) return false;
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

} // namespace rex::editor::core

