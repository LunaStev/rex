#include "ElementTree.h"

#include <algorithm>

namespace rex::ui::runtime::tree {

void ElementTree::clear() {
    records_.clear();
    indexById_.clear();
}

bool ElementTree::has(core::ElementId id) const {
    return indexById_.find(id) != indexById_.end();
}

ElementRecord* ElementTree::find(core::ElementId id) {
    const auto it = indexById_.find(id);
    if (it == indexById_.end()) return nullptr;
    return &records_[it->second];
}

const ElementRecord* ElementTree::find(core::ElementId id) const {
    const auto it = indexById_.find(id);
    if (it == indexById_.end()) return nullptr;
    return &records_[it->second];
}

void ElementTree::upsert(const ElementRecord& record) {
    if (record.id == core::kInvalidElementId) return;

    const auto it = indexById_.find(record.id);
    if (it == indexById_.end()) {
        ElementRecord inserted = record;
        if (inserted.revision == 0) inserted.revision = 1;
        indexById_[inserted.id] = records_.size();
        records_.push_back(inserted);
        return;
    }

    ElementRecord& existing = records_[it->second];
    ElementRecord updated = record;
    if (updated.revision <= existing.revision) {
        updated.revision = existing.revision + 1;
    }
    existing = updated;
}

void ElementTree::erase(core::ElementId id) {
    const auto it = indexById_.find(id);
    if (it == indexById_.end()) return;

    const std::size_t idx = it->second;
    const std::size_t last = records_.size() - 1;
    if (idx != last) {
        std::swap(records_[idx], records_[last]);
        indexById_[records_[idx].id] = idx;
    }
    records_.pop_back();
    indexById_.erase(it);
}

const std::vector<ElementRecord>& ElementTree::records() const {
    return records_;
}

} // namespace rex::ui::runtime::tree

