#include "EditorStateStore.h"

#include <utility>

namespace rex::editor::core {

EditorState& EditorStateStore::state() {
    return state_;
}

const EditorState& EditorStateStore::state() const {
    return state_;
}

ui::framework::state::UIStateStore& EditorStateStore::rawStore() {
    return store_;
}

const ui::framework::state::UIStateStore& EditorStateStore::rawStore() const {
    return store_;
}

void EditorStateStore::beginBatch() {
    store_.beginBatch();
}

void EditorStateStore::endBatch() {
    store_.endBatch();
}

EditorStateStore::SubscriptionId EditorStateStore::subscribe(const std::string& pathPrefix, Callback callback) {
    return store_.subscribe(pathPrefix, std::move(callback));
}

void EditorStateStore::unsubscribe(SubscriptionId id) {
    store_.unsubscribe(id);
}

} // namespace rex::editor::core
