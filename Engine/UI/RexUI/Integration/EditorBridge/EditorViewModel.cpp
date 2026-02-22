#include "EditorViewModel.h"

#include "../../Framework/Binding/BindingContext.h"
#include "../../Framework/State/UIStateStore.h"
#include "EditorUIState.h"

namespace rex::ui::integration::editorbridge {

EditorViewModel::EditorViewModel(
    framework::state::UIStateStore* stateStore,
    framework::binding::BindingEngine* bindingEngine,
    framework::binding::BindingContext* bindingContext)
    : stateStore_(stateStore)
    , bindingEngine_(bindingEngine)
    , bindingContext_(bindingContext) {}

void EditorViewModel::initialize() {
    if (!stateStore_) return;

    const EditorUIStatePaths paths{};
    stateStore_->set(paths.selectedEntity, std::int64_t(0));
    stateStore_->set(paths.activeViewport, std::string("Scene"));
    stateStore_->set(paths.gizmoMode, std::string("Move"));
    stateStore_->set(paths.hierarchyFilter, std::string());
    stateStore_->set(paths.undoAvailable, false);
    stateStore_->set(paths.redoAvailable, false);
}

void EditorViewModel::bindHierarchy() {
    if (!bindingEngine_ || !bindingContext_) return;
    bindingEngine_->evaluate(*bindingContext_);
}

void EditorViewModel::bindInspector() {
    if (!bindingEngine_ || !bindingContext_) return;
    bindingEngine_->evaluate(*bindingContext_);
}

void EditorViewModel::bindToolbar() {
    if (!bindingEngine_ || !bindingContext_) return;
    bindingEngine_->evaluate(*bindingContext_);
}

} // namespace rex::ui::integration::editorbridge

