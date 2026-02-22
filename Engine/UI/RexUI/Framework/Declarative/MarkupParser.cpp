#include "MarkupParser.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace rex::ui::framework::declarative {

namespace {
std::string trim(const std::string& s) {
    std::size_t begin = 0;
    while (begin < s.size() && std::isspace(static_cast<unsigned char>(s[begin]))) ++begin;

    std::size_t end = s.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(begin, end - begin);
}
} // namespace

ViewNodeDesc MarkupParser::parseText(const std::string& markupText) const {
    std::istringstream in(markupText);
    std::vector<ViewNodeDesc> nodeStack;
    std::vector<int> indentStack;

    ViewNodeDesc root{};
    root.type = "Panel";
    root.key = "root";
    nodeStack.push_back(root);
    indentStack.push_back(-1);

    std::string line;
    while (std::getline(in, line)) {
        if (trim(line).empty()) continue;

        int indent = 0;
        while (indent < static_cast<int>(line.size()) && line[indent] == ' ') ++indent;
        const std::string content = trim(line);

        std::istringstream row(content);
        ViewNodeDesc node{};
        row >> node.type;
        if (node.type.empty()) continue;
        row >> node.key;
        if (node.key.empty()) node.key = node.type;

        while (!indentStack.empty() && indentStack.back() >= indent) {
            if (nodeStack.size() <= 1) break;
            ViewNodeDesc completed = std::move(nodeStack.back());
            nodeStack.pop_back();
            indentStack.pop_back();
            nodeStack.back().children.push_back(std::move(completed));
        }

        nodeStack.push_back(std::move(node));
        indentStack.push_back(indent);
    }

    while (nodeStack.size() > 1) {
        ViewNodeDesc completed = std::move(nodeStack.back());
        nodeStack.pop_back();
        indentStack.pop_back();
        nodeStack.back().children.push_back(std::move(completed));
    }

    return nodeStack.front();
}

ViewNodeDesc MarkupParser::parseFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return {};
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return parseText(ss.str());
}

} // namespace rex::ui::framework::declarative
