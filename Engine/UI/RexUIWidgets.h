#pragma once
#include "RexUI.h"
#include <functional>

namespace rex::ui {

class Panel : public Widget {
public:
    Panel();
};

class Label : public Widget {
public:
    explicit Label(const std::string& text);
    void setText(const std::string& t) { m_text = t; }
    void draw(UIContext& ctx) override;

private:
    std::string m_text;
};

class Button : public Widget {
public:
    explicit Button(const std::string& text);
    void setOnClick(std::function<void()> cb) { m_onClick = std::move(cb); }

    void draw(UIContext& ctx) override;
    bool onMouseDown(UIContext& ctx, const Vec2& p, int button) override;
    bool onMouseUp(UIContext& ctx, const Vec2& p, int button) override;
    bool onMouseMove(UIContext& ctx, const Vec2& p) override;

private:
    std::string m_text;
    std::function<void()> m_onClick;
    bool m_hover = false;
    bool m_pressed = false;
};

class TextBox : public Widget {
public:
    TextBox();
    void setText(const std::string& t) { m_text = t; }
    const std::string& text() const { return m_text; }
    void setOnCommit(std::function<void(const std::string&)> cb) { m_onCommit = std::move(cb); }

    void draw(UIContext& ctx) override;
    bool onMouseDown(UIContext& ctx, const Vec2& p, int button) override;
    bool onKeyDown(UIContext& ctx, int key) override;
    bool onTextInput(UIContext& ctx, const std::string& text) override;

private:
    std::string m_text;
    size_t m_cursor = 0;
    bool m_focused = false;
    std::function<void(const std::string&)> m_onCommit;
};

class TreeView : public Widget {
public:
    struct Node {
        std::string label;
        std::vector<Node> children;
        bool expanded = true;
        int id = 0;
    };

    TreeView();
    void setNodes(std::vector<Node> nodes) { m_nodes = std::move(nodes); }
    void setOnSelect(std::function<void(int)> cb) { m_onSelect = std::move(cb); }

    void draw(UIContext& ctx) override;
    bool onMouseDown(UIContext& ctx, const Vec2& p, int button) override;

private:
    void drawNode(UIContext& ctx, const Node& n, int depth, float& y);
    bool hitTestNode(const Node& n, int depth, float& y, const Vec2& p, int& outId);

    std::vector<Node> m_nodes;
    int m_selectedId = -1;
    std::function<void(int)> m_onSelect;
    float m_rowHeight = 18.0f;
    float m_indent = 14.0f;
};

class PropertyInspector : public Widget {
public:
    struct Row {
        std::string name;
        std::string value;
    };

    PropertyInspector();
    void setRows(std::vector<Row> rows) { m_rows = std::move(rows); }

    void draw(UIContext& ctx) override;

private:
    std::vector<Row> m_rows;
    float m_rowHeight = 20.0f;
    float m_split = 0.45f;
};

class TabView : public Widget {
public:
    struct Tab {
        std::string title;
        std::unique_ptr<Widget> content;
    };

    TabView();
    void addTab(const std::string& title, std::unique_ptr<Widget> content);
    int activeIndex() const { return m_active; }

    void layout(UIContext& ctx, const UIRect& r) override;
    void draw(UIContext& ctx) override;
    bool onMouseDown(UIContext& ctx, const Vec2& p, int button) override;

private:
    std::vector<Tab> m_tabs;
    int m_active = 0;
    float m_tabHeight = 24.0f;
};

class Splitter : public Widget {
public:
    enum class Orientation { Horizontal, Vertical };
    Splitter(Orientation o);
    void setRatio(float r) { m_ratio = r; }

    void layout(UIContext& ctx, const UIRect& r) override;
    void draw(UIContext& ctx) override;

private:
    Orientation m_orientation;
    float m_ratio = 0.5f;
};

} // namespace rex::ui
