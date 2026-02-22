#include "AssetRegistry.h"

namespace rex::editor::asset {

bool AssetRegistry::registerAsset(const AssetRecord& record) {
    if (record.id.empty() || record.path.empty()) return false;
    byId_[record.id] = record;
    return true;
}

bool AssetRegistry::unregisterAsset(const std::string& assetId) {
    return byId_.erase(assetId) > 0;
}

std::optional<AssetRecord> AssetRegistry::findById(const std::string& assetId) const {
    const auto it = byId_.find(assetId);
    if (it == byId_.end()) return std::nullopt;
    return it->second;
}

std::vector<AssetRecord> AssetRegistry::queryByPathPrefix(const std::string& prefix) const {
    std::vector<AssetRecord> out;
    for (const auto& [id, record] : byId_) {
        (void)id;
        if (record.path.rfind(prefix, 0) == 0) {
            out.push_back(record);
        }
    }
    return out;
}

std::vector<AssetRecord> AssetRegistry::queryByType(const std::string& type) const {
    std::vector<AssetRecord> out;
    for (const auto& [id, record] : byId_) {
        (void)id;
        if (record.type == type) {
            out.push_back(record);
        }
    }
    return out;
}

} // namespace rex::editor::asset

