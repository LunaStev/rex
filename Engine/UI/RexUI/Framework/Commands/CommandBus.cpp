#include "CommandBus.h"

#include "UndoRedoStack.h"

namespace rex::ui::framework::commands {

CommandBus::CommandBus(UndoRedoStack* stack)
    : stack_(stack) {}

bool CommandBus::execute(const std::shared_ptr<UICommand>& command, UICommandContext& context) {
    if (!command) return false;
    if (!command->execute(context)) return false;

    if (stack_) {
        stack_->push(command);
    }

    for (const auto& [id, listener] : listeners_) {
        (void)id;
        if (listener) listener(*command);
    }
    return true;
}

bool CommandBus::undo(UICommandContext& context) {
    if (!stack_ || !stack_->canUndo()) return false;
    auto command = stack_->popUndo();
    if (!command) return false;
    if (command->undo(context)) {
        return true;
    }

    // Undo 실패 시 스택 정합성 복원.
    stack_->popRedo();
    return false;
}

bool CommandBus::redo(UICommandContext& context) {
    if (!stack_ || !stack_->canRedo()) return false;
    auto command = stack_->popRedo();
    if (!command) return false;
    if (command->redo(context)) {
        return true;
    }

    // Redo 실패 시 스택 정합성 복원.
    stack_->popUndo();
    return false;
}

CommandBus::ListenerId CommandBus::addListener(Listener listener) {
    const ListenerId id = nextListenerId_++;
    listeners_[id] = std::move(listener);
    return id;
}

void CommandBus::removeListener(ListenerId id) {
    listeners_.erase(id);
}

} // namespace rex::ui::framework::commands

