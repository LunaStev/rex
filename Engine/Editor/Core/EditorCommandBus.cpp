#include "EditorCommandBus.h"

namespace rex::editor::core {

EditorCommandBus::EditorCommandBus() = default;

bool EditorCommandBus::execute(const std::shared_ptr<ui::framework::commands::UICommand>& command) {
    return bus_.execute(command, context_);
}

bool EditorCommandBus::undo() {
    return bus_.undo(context_);
}

bool EditorCommandBus::redo() {
    return bus_.redo(context_);
}

bool EditorCommandBus::canUndo() const {
    return undoRedo_.canUndo();
}

bool EditorCommandBus::canRedo() const {
    return undoRedo_.canRedo();
}

ui::framework::commands::CommandBus& EditorCommandBus::rawBus() {
    return bus_;
}

const ui::framework::commands::CommandBus& EditorCommandBus::rawBus() const {
    return bus_;
}

} // namespace rex::editor::core

