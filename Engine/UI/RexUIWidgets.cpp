#include "RexUIWidgets.h"

namespace rex::ui {

Panel::Panel() {
    UIStyle s;
    s.bg = {0.15f, 0.15f, 0.15f, 1.0f};
    s.border = {0.05f, 0.05f, 0.05f, 1.0f};
    s.borderWidth = 1.0f;
    s.padding = 6.0f;
    setStyle(s);
    setLayout(UILayout::Column);
}

Label::Label(const std::string& text) : m_text(text) {
    UIStyle s;
    s.text = {0.9f, 0.9f, 0.9f, 1.0f};
    setStyle(s);
    setPreferredSize({0, 18});
}

void Label::draw(UIContext& ctx) {
    Widget::draw(ctx);
    if (!m_text.empty()) {
        ctx.pushText(m_rect, m_style.text, m_text);
    }
}

Button::Button(const std::string& text) : m_text(text) {
    UIStyle s;
    s.bg = {0.22f, 0.22f, 0.22f, 1.0f};
    s.text = {0.95f, 0.95f, 0.95f, 1.0f};
    s.padding = 6.0f;
    setStyle(s);
    setPreferredSize({0, 24});
}

void Button::draw(UIContext& ctx) {
    UIStyle s = m_style;
    if (m_pressed) s.bg = {0.18f, 0.18f, 0.18f, 1.0f};
    else if (m_hover) s.bg = {0.28f, 0.28f, 0.28f, 1.0f};

    if (s.bg.a > 0.0f) ctx.pushRect(m_rect, s.bg);
    if (!m_text.empty()) ctx.pushText(m_rect, s.text, m_text);
    for (auto& c : m_children) c->draw(ctx);
}

bool Button::onMouseDown(UIContext& ctx, const Vec2& p, int button) {
    (void)ctx;
    if (!m_rect.contains(p) || button != 0)
        return false;
    m_pressed = true;
    return true;
}

bool Button::onMouseUp(UIContext& ctx, const Vec2& p, int button) {
    (void)ctx;
    if (button != 0)
        return false;
    bool wasPressed = m_pressed;
    m_pressed = false;
    if (wasPressed && m_rect.contains(p)) {
        if (m_onClick) m_onClick();
        return true;
    }
    return false;
}

bool Button::onMouseMove(UIContext& ctx, const Vec2& p) {
    (void)ctx;
    bool inside = m_rect.contains(p);
    m_hover = inside;
    return inside;
}

TextBox::TextBox() {
    UIStyle s;
    s.bg = {0.10f, 0.10f, 0.10f, 1.0f};
    s.border = {0.25f, 0.25f, 0.25f, 1.0f};
    s.borderWidth = 1.0f;
    s.text = {0.95f, 0.95f, 0.95f, 1.0f};
    s.padding = 6.0f;
    setStyle(s);
    setPreferredSize({0, 22});
}

void TextBox::draw(UIContext& ctx) {
    UIColor bg = m_style.bg;
    if (m_focused) bg = {0.14f, 0.14f, 0.14f, 1.0f};
    ctx.pushRect(m_rect, bg);
    ctx.pushText(m_rect, m_style.text, m_text);

    if (m_focused) {
        float caretX = m_rect.x + m_style.padding + float(m_cursor) * 8.0f;
        UIRect caret{caretX, m_rect.y + 4.0f, 1.0f, m_rect.h - 8.0f};
        ctx.pushRect(caret, {0.9f, 0.9f, 0.9f, 1.0f});
    }
}

bool TextBox::onMouseDown(UIContext& ctx, const Vec2& p, int button) {
    if (button != 0) return false;
    if (!m_rect.contains(p)) return false;
    m_focused = true;
    ctx.setFocused(this);
    return true;
}

bool TextBox::onKeyDown(UIContext& ctx, int key) {
    (void)ctx;
    if (!m_focused) return false;

    if (key == (int)UIKey::Backspace) {
        if (!m_text.empty() && m_cursor > 0) {
            m_text.erase(m_cursor - 1, 1);
            m_cursor--;
        }
        return true;
    }
    if (key == (int)UIKey::DeleteKey) {
        if (m_cursor < m_text.size()) {
            m_text.erase(m_cursor, 1);
        }
        return true;
    }
    if (key == (int)UIKey::Left) {
        if (m_cursor > 0) m_cursor--;
        return true;
    }
    if (key == (int)UIKey::Right) {
        if (m_cursor < m_text.size()) m_cursor++;
        return true;
    }
    if (key == (int)UIKey::Enter) {
        if (m_onCommit) m_onCommit(m_text);
        return true;
    }

    return false;
}

bool TextBox::onTextInput(UIContext& ctx, const std::string& text) {
    (void)ctx;
    if (!m_focused) return false;
    if (text.empty()) return false;
    m_text.insert(m_cursor, text);
    m_cursor += text.size();
    return true;
}

TreeView::TreeView() {
    UIStyle s;
    s.bg = {0.12f, 0.12f, 0.12f, 1.0f};
    s.text = {0.9f, 0.9f, 0.9f, 1.0f};
    s.padding = 6.0f;
    setStyle(s);
    setPreferredSize({0, 0});
}

void TreeView::draw(UIContext& ctx) {
    if (m_style.bg.a > 0.0f) ctx.pushRect(m_rect, m_style.bg);
    float y = m_rect.y + m_style.padding;
    for (const auto& n : m_nodes) {
        drawNode(ctx, n, 0, y);
    }
}

void TreeView::drawNode(UIContext& ctx, const Node& n, int depth, float& y) {
    UIRect row{m_rect.x + m_style.padding, y, m_rect.w - m_style.padding * 2.0f, m_rowHeight};
    if (n.id == m_selectedId) {
        ctx.pushRect(row, {0.2f, 0.3f, 0.5f, 1.0f});
    }

    float textX = row.x + depth * m_indent + 4.0f;
    UIRect textRect{textX, row.y, row.w - depth * m_indent, row.h};
    ctx.pushText(textRect, m_style.text, n.label);

    y += m_rowHeight;
    if (n.expanded) {
        for (const auto& c : n.children) {
            drawNode(ctx, c, depth + 1, y);
        }
    }
}

bool TreeView::hitTestNode(const Node& n, int depth, float& y, const Vec2& p, int& outId) {
    UIRect row{m_rect.x + m_style.padding, y, m_rect.w - m_style.padding * 2.0f, m_rowHeight};
    if (row.contains(p)) {
        outId = n.id;
        return true;
    }
    y += m_rowHeight;
    if (n.expanded) {
        for (const auto& c : n.children) {
            if (hitTestNode(c, depth + 1, y, p, outId))
                return true;
        }
    }
    return false;
}

bool TreeView::onMouseDown(UIContext& ctx, const Vec2& p, int button) {
    (void)ctx;
    if (button != 0 || !m_rect.contains(p))
        return false;
    float y = m_rect.y + m_style.padding;
    int id = -1;
    for (const auto& n : m_nodes) {
        if (hitTestNode(n, 0, y, p, id)) {
            m_selectedId = id;
            if (m_onSelect) m_onSelect(id);
            return true;
        }
    }
    return false;
}

PropertyInspector::PropertyInspector() {
    UIStyle s;
    s.bg = {0.13f, 0.13f, 0.13f, 1.0f};
    s.text = {0.9f, 0.9f, 0.9f, 1.0f};
    s.padding = 6.0f;
    setStyle(s);
    setLayout(UILayout::Column);
}

void PropertyInspector::draw(UIContext& ctx) {
    if (m_style.bg.a > 0.0f) ctx.pushRect(m_rect, m_style.bg);
    float y = m_rect.y + m_style.padding;
    float nameW = m_rect.w * m_split;
    float valueW = m_rect.w - nameW - m_style.padding * 2.0f;

    for (const auto& r : m_rows) {
        UIRect nameRect{m_rect.x + m_style.padding, y, nameW, m_rowHeight};
        UIRect valueRect{m_rect.x + m_style.padding + nameW, y, valueW, m_rowHeight};
        ctx.pushText(nameRect, m_style.text, r.name);
        ctx.pushText(valueRect, m_style.text, r.value);
        y += m_rowHeight;
    }
}

TabView::TabView() {
    UIStyle s;
    s.bg = {0.12f, 0.12f, 0.12f, 1.0f};
    s.text = {0.9f, 0.9f, 0.9f, 1.0f};
    s.padding = 0.0f;
    setStyle(s);
    setLayout(UILayout::Absolute);
}

void TabView::addTab(const std::string& title, std::unique_ptr<Widget> content) {
    Tab t;
    t.title = title;
    t.content = std::move(content);
    m_tabs.emplace_back(std::move(t));
}

void TabView::layout(UIContext& ctx, const UIRect& r) {
    m_rect = r;
    if (m_tabs.empty()) return;
    if (m_active < 0 || m_active >= (int)m_tabs.size()) m_active = 0;
    UIRect content{r.x, r.y + m_tabHeight, r.w, r.h - m_tabHeight};
    m_tabs[m_active].content->layout(ctx, content);
}

void TabView::draw(UIContext& ctx) {
    ctx.pushRect({m_rect.x, m_rect.y, m_rect.w, m_tabHeight}, {0.10f, 0.10f, 0.10f, 1.0f});
    float x = m_rect.x;
    float tabW = 100.0f;
    for (size_t i = 0; i < m_tabs.size(); ++i) {
        UIColor c = (int)i == m_active ? UIColor{0.18f, 0.18f, 0.18f, 1.0f} : UIColor{0.12f, 0.12f, 0.12f, 1.0f};
        UIRect tabRect{x, m_rect.y, tabW, m_tabHeight};
        ctx.pushRect(tabRect, c);
        ctx.pushText(tabRect, m_style.text, m_tabs[i].title);
        x += tabW;
    }
    if (!m_tabs.empty()) m_tabs[m_active].content->draw(ctx);
}

bool TabView::onMouseDown(UIContext& ctx, const Vec2& p, int button) {
    (void)ctx;
    if (button != 0) return false;
    if (p.y < m_rect.y || p.y > m_rect.y + m_tabHeight) return false;
    float x = m_rect.x;
    float tabW = 100.0f;
    for (size_t i = 0; i < m_tabs.size(); ++i) {
        UIRect tabRect{x, m_rect.y, tabW, m_tabHeight};
        if (tabRect.contains(p)) {
            m_active = (int)i;
            return true;
        }
        x += tabW;
    }
    return false;
}

Splitter::Splitter(Orientation o) : m_orientation(o) {
    UIStyle s;
    s.bg = {0.0f, 0.0f, 0.0f, 0.0f};
    setStyle(s);
    setLayout(UILayout::Absolute);
}

void Splitter::layout(UIContext& ctx, const UIRect& r) {
    m_rect = r;
    if (m_children.size() < 2) return;
    if (m_orientation == Orientation::Horizontal) {
        float leftW = r.w * m_ratio;
        m_children[0]->layout(ctx, {r.x, r.y, leftW, r.h});
        m_children[1]->layout(ctx, {r.x + leftW, r.y, r.w - leftW, r.h});
    } else {
        float topH = r.h * m_ratio;
        m_children[0]->layout(ctx, {r.x, r.y, r.w, topH});
        m_children[1]->layout(ctx, {r.x, r.y + topH, r.w, r.h - topH});
    }
}

void Splitter::draw(UIContext& ctx) {
    for (auto& c : m_children) c->draw(ctx);
}

} // namespace rex::ui
