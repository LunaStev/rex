#include "TransactionSystem.h"

namespace rex::editor::core {

void TransactionSystem::begin(const std::string& label) {
    if (open_) return;
    open_ = true;
    current_ = {};
    current_.label = label;
}

void TransactionSystem::markCommandPushed() {
    if (!open_) return;
    ++current_.commandCount;
}

TransactionRecord TransactionSystem::end() {
    if (!open_) return {};
    open_ = false;
    history_.push_back(current_);
    return current_;
}

bool TransactionSystem::inTransaction() const {
    return open_;
}

const std::vector<TransactionRecord>& TransactionSystem::history() const {
    return history_;
}

} // namespace rex::editor::core

