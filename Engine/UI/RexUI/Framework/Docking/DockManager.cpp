#include "DockManager.h"

#include <algorithm>
#include <utility>

namespace rex::ui::framework::docking {

DockNodeId DockManager::createNode(DockNodeType type, DockNodeId parentId) {
    const DockNodeId id = nextNodeId_++;
    DockNode node{};
    node.id = id;
    node.type = type;
    node.parent = parentId;
    nodes_[id] = std::move(node);
    return id;
}

void DockManager::removePanelFromNode(DockNode& node, DockPanelId panelId) {
    node.tabs.erase(std::remove(node.tabs.begin(), node.tabs.end(), panelId), node.tabs.end());
    if (node.activeTab == panelId) {
        node.activeTab = node.tabs.empty() ? 0 : node.tabs.front();
    }
}

DockManager::DockManager() {
    root_ = createNode(DockNodeType::TabStack, 0);
}

DockNodeId DockManager::root() const {
    return root_;
}

DockPanelId DockManager::createPanel(const std::string& panelName) {
    const DockPanelId id = nextPanelId_++;
    panels_[id] = panelName;

    auto it = nodes_.find(root_);
    if (it != nodes_.end()) {
        it->second.tabs.push_back(id);
        if (it->second.activeTab == 0) it->second.activeTab = id;
    }
    return id;
}

bool DockManager::destroyPanel(DockPanelId panelId) {
    if (panels_.erase(panelId) == 0) return false;
    for (auto& [id, node] : nodes_) {
        (void)id;
        removePanelFromNode(node, panelId);
    }
    return true;
}

bool DockManager::dock(DockPanelId panelId, DockNodeId targetNode, DockSplitDirection direction, float ratio) {
    if (panels_.find(panelId) == panels_.end()) return false;
    auto targetIt = nodes_.find(targetNode);
    if (targetIt == nodes_.end()) return false;

    undock(panelId);

    DockNode& target = targetIt->second;
    if (target.type == DockNodeType::TabStack) {
        if (ratio > 0.0f && ratio < 1.0f) {
            if (!split(targetNode, direction, ratio)) return false;
            DockNode& parent = nodes_[targetNode];
            DockNode& second = nodes_[parent.secondChild];
            second.tabs.push_back(panelId);
            second.activeTab = panelId;
            return true;
        }
        target.tabs.push_back(panelId);
        target.activeTab = panelId;
        return true;
    }

    DockNodeId destination = target.firstChild != 0 ? target.firstChild : target.secondChild;
    auto destIt = nodes_.find(destination);
    if (destIt == nodes_.end()) return false;
    destIt->second.tabs.push_back(panelId);
    destIt->second.activeTab = panelId;
    return true;
}

bool DockManager::undock(DockPanelId panelId) {
    bool removed = false;
    for (auto& [id, node] : nodes_) {
        (void)id;
        const std::size_t before = node.tabs.size();
        removePanelFromNode(node, panelId);
        removed = removed || (node.tabs.size() != before);
    }
    return removed;
}

bool DockManager::setActiveTab(DockNodeId tabStackNode, DockPanelId panelId) {
    auto it = nodes_.find(tabStackNode);
    if (it == nodes_.end()) return false;
    DockNode& node = it->second;
    if (node.type != DockNodeType::TabStack) return false;
    if (std::find(node.tabs.begin(), node.tabs.end(), panelId) == node.tabs.end()) return false;
    node.activeTab = panelId;
    return true;
}

bool DockManager::split(DockNodeId nodeId, DockSplitDirection direction, float ratio) {
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) return false;
    DockNode& node = it->second;
    if (ratio <= 0.05f || ratio >= 0.95f) return false;
    if (node.type == DockNodeType::Split) return false;

    const DockNodeId first = createNode(DockNodeType::TabStack, nodeId);
    const DockNodeId second = createNode(DockNodeType::TabStack, nodeId);

    nodes_[first].tabs = std::move(node.tabs);
    nodes_[first].activeTab = node.activeTab;

    node.type = DockNodeType::Split;
    node.splitDirection = direction;
    node.splitRatio = ratio;
    node.firstChild = first;
    node.secondChild = second;
    node.tabs.clear();
    node.activeTab = 0;
    return true;
}

bool DockManager::merge(DockNodeId nodeId) {
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) return false;
    DockNode& node = it->second;
    if (node.type != DockNodeType::Split) return false;

    auto firstIt = nodes_.find(node.firstChild);
    auto secondIt = nodes_.find(node.secondChild);
    if (firstIt == nodes_.end() || secondIt == nodes_.end()) return false;

    node.type = DockNodeType::TabStack;
    node.tabs = firstIt->second.tabs;
    node.tabs.insert(node.tabs.end(), secondIt->second.tabs.begin(), secondIt->second.tabs.end());
    node.activeTab = node.tabs.empty() ? 0 : node.tabs.front();
    node.firstChild = 0;
    node.secondChild = 0;
    node.splitRatio = 0.5f;

    nodes_.erase(firstIt);
    nodes_.erase(secondIt);
    return true;
}

const DockNode* DockManager::findNode(DockNodeId id) const {
    const auto it = nodes_.find(id);
    if (it == nodes_.end()) return nullptr;
    return &it->second;
}

const std::unordered_map<DockNodeId, DockNode>& DockManager::nodes() const {
    return nodes_;
}

const std::unordered_map<DockPanelId, std::string>& DockManager::panels() const {
    return panels_;
}

} // namespace rex::ui::framework::docking

