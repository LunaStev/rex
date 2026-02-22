#include "SelectionManager.h"

namespace rex::editor::core {

void SelectionManager::clear() {
    selectedEntities_.clear();
    selectedAssets_.clear();
}

void SelectionManager::selectEntity(std::uint64_t entityId, bool additive) {
    if (!additive) {
        selectedEntities_.clear();
    }
    selectedEntities_.insert(entityId);
}

void SelectionManager::deselectEntity(std::uint64_t entityId) {
    selectedEntities_.erase(entityId);
}

bool SelectionManager::isEntitySelected(std::uint64_t entityId) const {
    return selectedEntities_.find(entityId) != selectedEntities_.end();
}

std::vector<std::uint64_t> SelectionManager::selectedEntities() const {
    return {selectedEntities_.begin(), selectedEntities_.end()};
}

void SelectionManager::selectAsset(const std::string& assetId, bool additive) {
    if (!additive) {
        selectedAssets_.clear();
    }
    selectedAssets_.insert(assetId);
}

void SelectionManager::deselectAsset(const std::string& assetId) {
    selectedAssets_.erase(assetId);
}

bool SelectionManager::isAssetSelected(const std::string& assetId) const {
    return selectedAssets_.find(assetId) != selectedAssets_.end();
}

std::vector<std::string> SelectionManager::selectedAssets() const {
    return {selectedAssets_.begin(), selectedAssets_.end()};
}

} // namespace rex::editor::core

