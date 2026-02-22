#include "DiffEngine.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "../Tree/WidgetTree.h"

namespace rex::ui::runtime::diff {

namespace {
using RecordMap = std::unordered_map<core::ElementId, tree::ElementRecord>;

RecordMap makeRecordMap(const tree::WidgetTree& widgetTree) {
    RecordMap out;
    for (const auto& r : widgetTree.elements().records()) {
        out[r.id] = r;
    }
    return out;
}

bool rectChanged(const core::Rect& a, const core::Rect& b) {
    return a.x != b.x || a.y != b.y || a.w != b.w || a.h != b.h;
}
} // namespace

std::vector<PatchOp> DiffEngine::diff(const tree::WidgetTree& previousTree, const tree::WidgetTree& nextTree) const {
    std::vector<PatchOp> ops;
    const RecordMap prev = makeRecordMap(previousTree);
    const RecordMap next = makeRecordMap(nextTree);

    ops.reserve(prev.size() + next.size());

    for (const auto& [id, nextRecord] : next) {
        const auto it = prev.find(id);
        if (it == prev.end()) {
            ops.push_back({
                PatchOpType::Add,
                id,
                nextRecord.parent,
                "node",
                ""
            });
            continue;
        }

        const auto& prevRecord = it->second;
        if (prevRecord.parent != nextRecord.parent) {
            ops.push_back({
                PatchOpType::Move,
                id,
                nextRecord.parent,
                "parent",
                ""
            });
        }
        if (prevRecord.visible != nextRecord.visible || rectChanged(prevRecord.rect, nextRecord.rect)) {
            ops.push_back({
                PatchOpType::UpdateProperty,
                id,
                nextRecord.parent,
                "layout",
                ""
            });
        }
    }

    for (const auto& [id, prevRecord] : prev) {
        (void)prevRecord;
        if (next.find(id) != next.end()) continue;
        ops.push_back({
            PatchOpType::Remove,
            id,
            core::kInvalidElementId,
            "node",
            ""
        });
    }

    std::sort(ops.begin(), ops.end(), [](const PatchOp& a, const PatchOp& b) {
        return static_cast<int>(a.type) < static_cast<int>(b.type);
    });
    return ops;
}

bool DiffEngine::apply(const std::vector<PatchOp>& patchOps, tree::WidgetTree& targetTree) const {
    auto& elements = targetTree.elements();
    for (const auto& op : patchOps) {
        switch (op.type) {
            case PatchOpType::Add:
            case PatchOpType::Replace:
            case PatchOpType::UpdateProperty:
            case PatchOpType::Move: {
                tree::ElementRecord record{};
                if (const auto* existing = elements.find(op.target)) {
                    record = *existing;
                }
                record.id = op.target;
                if (op.parent != core::kInvalidElementId) {
                    record.parent = op.parent;
                }
                record.revision += 1;
                elements.upsert(record);
                break;
            }
            case PatchOpType::Remove:
                elements.erase(op.target);
                break;
        }
    }
    return true;
}

} // namespace rex::ui::runtime::diff

