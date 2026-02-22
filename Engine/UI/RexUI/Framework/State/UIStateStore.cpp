#include "UIStateStore.h"

namespace rex::ui::framework::state {

bool UIStateStore::pathMatchesPrefix(const StatePath& path, const StatePath& prefix) {
    if (prefix.empty()) return true;
    if (path.size() < prefix.size()) return false;
    if (path.compare(0, prefix.size(), prefix) != 0) return false;
    if (path.size() == prefix.size()) return true;
    return path[prefix.size()] == '.';
}

void UIStateStore::notifyPath(const StatePath& path, const StateValue& value) {
    for (const auto& [id, sub] : subscriptions_) {
        (void)id;
        if (!pathMatchesPrefix(path, sub.pathPrefix)) continue;
        if (sub.callback) {
            sub.callback(path, value);
        }
    }
}

bool UIStateStore::has(const StatePath& path) const {
    return values_.find(path) != values_.end();
}

std::optional<StateValue> UIStateStore::get(const StatePath& path) const {
    const auto it = values_.find(path);
    if (it == values_.end()) {
        return std::nullopt;
    }
    return it->second;
}

void UIStateStore::set(const StatePath& path, const StateValue& value) {
    values_[path] = value;
    if (batching_) {
        batchedDirtyPaths_.insert(path);
    } else {
        notifyPath(path, value);
    }
}

void UIStateStore::remove(const StatePath& path) {
    values_.erase(path);
    if (batching_) {
        batchedDirtyPaths_.insert(path);
    } else {
        notifyPath(path, StateValue{});
    }
}

UIStateStore::SubscriptionId UIStateStore::subscribe(const StatePath& pathPrefix, Callback callback) {
    const SubscriptionId id = nextSubscriptionId_++;
    subscriptions_[id] = {pathPrefix, std::move(callback)};
    return id;
}

void UIStateStore::unsubscribe(SubscriptionId id) {
    subscriptions_.erase(id);
}

void UIStateStore::beginBatch() {
    batching_ = true;
}

void UIStateStore::endBatch() {
    if (!batching_) return;
    batching_ = false;

    for (const auto& path : batchedDirtyPaths_) {
        const auto it = values_.find(path);
        if (it == values_.end()) {
            notifyPath(path, StateValue{});
        } else {
            notifyPath(path, it->second);
        }
    }
    batchedDirtyPaths_.clear();
}

} // namespace rex::ui::framework::state

