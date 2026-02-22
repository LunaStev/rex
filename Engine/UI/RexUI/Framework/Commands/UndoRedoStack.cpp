#include "UndoRedoStack.h"

#include <algorithm>

namespace rex::ui::framework::commands {

void UndoRedoStack::setCapacity(std::size_t capacity) {
    capacity_ = std::max<std::size_t>(1, capacity);
    while (undo_.size() > capacity_) {
        undo_.erase(undo_.begin());
    }
    while (redo_.size() > capacity_) {
        redo_.erase(redo_.begin());
    }
}

std::size_t UndoRedoStack::capacity() const {
    return capacity_;
}

void UndoRedoStack::push(std::shared_ptr<UICommand> command) {
    if (!command) return;
    redo_.clear();
    if (undo_.size() >= capacity_) {
        undo_.erase(undo_.begin());
    }
    undo_.push_back(std::move(command));
}

bool UndoRedoStack::canUndo() const {
    return !undo_.empty();
}

bool UndoRedoStack::canRedo() const {
    return !redo_.empty();
}

std::shared_ptr<UICommand> UndoRedoStack::popUndo() {
    if (undo_.empty()) return {};
    auto cmd = undo_.back();
    undo_.pop_back();
    if (redo_.size() >= capacity_) {
        redo_.erase(redo_.begin());
    }
    redo_.push_back(cmd);
    return cmd;
}

std::shared_ptr<UICommand> UndoRedoStack::popRedo() {
    if (redo_.empty()) return {};
    auto cmd = redo_.back();
    redo_.pop_back();
    if (undo_.size() >= capacity_) {
        undo_.erase(undo_.begin());
    }
    undo_.push_back(cmd);
    return cmd;
}

void UndoRedoStack::clear() {
    undo_.clear();
    redo_.clear();
}

} // namespace rex::ui::framework::commands

