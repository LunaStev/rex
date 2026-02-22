#include "DependencyGraph.h"

namespace rex::editor::asset {

void DependencyGraph::addDependency(const std::string& fromAssetId, const std::string& toAssetId) {
    if (fromAssetId.empty() || toAssetId.empty()) return;
    outgoing_[fromAssetId].insert(toAssetId);
    incoming_[toAssetId].insert(fromAssetId);
}

void DependencyGraph::removeAsset(const std::string& assetId) {
    if (assetId.empty()) return;

    const auto outIt = outgoing_.find(assetId);
    if (outIt != outgoing_.end()) {
        for (const auto& dep : outIt->second) {
            incoming_[dep].erase(assetId);
        }
        outgoing_.erase(outIt);
    }

    const auto inIt = incoming_.find(assetId);
    if (inIt != incoming_.end()) {
        for (const auto& dep : inIt->second) {
            outgoing_[dep].erase(assetId);
        }
        incoming_.erase(inIt);
    }
}

std::vector<std::string> DependencyGraph::dependenciesOf(const std::string& assetId) const {
    std::vector<std::string> out;
    const auto it = outgoing_.find(assetId);
    if (it == outgoing_.end()) return out;
    out.insert(out.end(), it->second.begin(), it->second.end());
    return out;
}

std::vector<std::string> DependencyGraph::dependentsOf(const std::string& assetId) const {
    std::vector<std::string> out;
    const auto it = incoming_.find(assetId);
    if (it == incoming_.end()) return out;
    out.insert(out.end(), it->second.begin(), it->second.end());
    return out;
}

} // namespace rex::editor::asset

