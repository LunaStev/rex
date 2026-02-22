#include "DockSerializer.h"

#include <sstream>
#include <string>

#include "DockManager.h"

namespace rex::ui::framework::docking {

namespace {
const char* nodeTypeToString(DockNodeType type) {
    switch (type) {
        case DockNodeType::Split: return "Split";
        case DockNodeType::TabStack: return "TabStack";
        case DockNodeType::Floating: return "Floating";
    }
    return "TabStack";
}

DockNodeType parseNodeType(const std::string& token) {
    if (token == "Split") return DockNodeType::Split;
    if (token == "Floating") return DockNodeType::Floating;
    return DockNodeType::TabStack;
}
} // namespace

std::string DockSerializer::serialize(const DockManager& manager) const {
    std::ostringstream out;
    out << "root " << manager.root_ << "\n";

    for (const auto& [panelId, panelName] : manager.panels_) {
        out << "panel " << panelId << " " << panelName << "\n";
    }

    for (const auto& [nodeId, node] : manager.nodes_) {
        out << "node "
            << nodeId << " "
            << nodeTypeToString(node.type) << " "
            << node.parent << " "
            << node.firstChild << " "
            << node.secondChild << " "
            << static_cast<int>(node.splitDirection) << " "
            << node.splitRatio << " "
            << node.activeTab << " ";
        for (std::size_t i = 0; i < node.tabs.size(); ++i) {
            out << node.tabs[i];
            if (i + 1 < node.tabs.size()) out << ",";
        }
        out << "\n";
    }

    return out.str();
}

bool DockSerializer::deserialize(const std::string& data, DockManager& manager) const {
    std::istringstream in(data);

    manager.nodes_.clear();
    manager.panels_.clear();
    manager.root_ = 0;
    manager.nextNodeId_ = 1;
    manager.nextPanelId_ = 1;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::istringstream row(line);
        std::string kind;
        row >> kind;

        if (kind == "root") {
            row >> manager.root_;
            continue;
        }

        if (kind == "panel") {
            DockPanelId panelId = 0;
            std::string panelName;
            row >> panelId;
            row >> std::ws;
            std::getline(row, panelName);
            manager.panels_[panelId] = panelName;
            if (panelId >= manager.nextPanelId_) manager.nextPanelId_ = panelId + 1;
            continue;
        }

        if (kind == "node") {
            DockNode node{};
            std::string nodeTypeToken;
            int splitDir = 0;
            std::string tabs;
            row >> node.id >> nodeTypeToken >> node.parent >> node.firstChild >> node.secondChild
                >> splitDir >> node.splitRatio >> node.activeTab >> tabs;
            node.type = parseNodeType(nodeTypeToken);
            node.splitDirection = (splitDir == 0) ? DockSplitDirection::Horizontal : DockSplitDirection::Vertical;

            std::istringstream tabsStream(tabs);
            std::string token;
            while (std::getline(tabsStream, token, ',')) {
                if (token.empty()) continue;
                node.tabs.push_back(static_cast<DockPanelId>(std::stoull(token)));
            }

            manager.nodes_[node.id] = node;
            if (node.id >= manager.nextNodeId_) manager.nextNodeId_ = node.id + 1;
        }
    }

    if (manager.root_ == 0 || manager.nodes_.find(manager.root_) == manager.nodes_.end()) {
        manager.nodes_.clear();
        manager.panels_.clear();
        manager.root_ = manager.createNode(DockNodeType::TabStack, 0);
    }

    return true;
}

} // namespace rex::ui::framework::docking

