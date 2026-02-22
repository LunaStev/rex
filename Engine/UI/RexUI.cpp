#include "RexUI.h"

namespace rex::ui {

Widget::Widget() = default;
Widget::~Widget() = default;

void Widget::addChild(std::unique_ptr<Widget> child) {
    m_children.emplace_back(std::move(child));
}

Vec2 Widget::measure(UIContext& ctx, const Vec2& available) {
    (void)ctx;
    Vec2 size = m_preferred;
    if (size.x <= 0) size.x = available.x;
    if (size.y <= 0) size.y = available.y;
    return size;
}

void Widget::layout(UIContext& ctx, const UIRect& r) {
    (void)ctx;
    m_rect = r;

    if (m_children.empty())
        return;

    float innerX = r.x + m_style.padding;
    float innerY = r.y + m_style.padding;
    float innerW = r.w - m_style.padding * 2.0f;
    float innerH = r.h - m_style.padding * 2.0f;

    if (m_layout == UILayout::Absolute) {
        for (auto& c : m_children)
            c->layout(ctx, c->rect());
        return;
    }

    if (m_layout == UILayout::Row) {
        float flexSum = 0.0f;
        float fixed = 0.0f;
        for (auto& c : m_children) {
            Vec2 ms = c->measure(ctx, {innerW, innerH});
            if (c->m_flex > 0.0f) flexSum += c->m_flex;
            else fixed += ms.x;
        }
        float x = innerX;
        float rem = innerW - fixed;
        for (auto& c : m_children) {
            Vec2 ms = c->measure(ctx, {innerW, innerH});
            float w = (c->m_flex > 0.0f && flexSum > 0.0f) ? (rem * (c->m_flex / flexSum)) : ms.x;
            float h = (m_align == UIAlign::Stretch) ? innerH : ms.y;
            c->layout(ctx, {x, innerY, w, h});
            x += w;
        }
        return;
    }

    if (m_layout == UILayout::Column) {
        float flexSum = 0.0f;
        float fixed = 0.0f;
        for (auto& c : m_children) {
            Vec2 ms = c->measure(ctx, {innerW, innerH});
            if (c->m_flex > 0.0f) flexSum += c->m_flex;
            else fixed += ms.y;
        }
        float y = innerY;
        float rem = innerH - fixed;
        for (auto& c : m_children) {
            Vec2 ms = c->measure(ctx, {innerW, innerH});
            float h = (c->m_flex > 0.0f && flexSum > 0.0f) ? (rem * (c->m_flex / flexSum)) : ms.y;
            float w = (m_align == UIAlign::Stretch) ? innerW : ms.x;
            c->layout(ctx, {innerX, y, w, h});
            y += h;
        }
        return;
    }
}

void Widget::draw(UIContext& ctx) {
    if (m_style.bg.a > 0.0f) {
        ctx.pushRect(m_rect, m_style.bg);
    }
    for (auto& c : m_children)
        c->draw(ctx);
}

bool Widget::onMouseDown(UIContext& ctx, const Vec2& p, int button) {
    (void)ctx;
    (void)button;
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->rect().contains(p) && (*it)->onMouseDown(ctx, p, button))
            return true;
    }
    return false;
}

bool Widget::onMouseUp(UIContext& ctx, const Vec2& p, int button) {
    (void)ctx;
    (void)button;
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->rect().contains(p) && (*it)->onMouseUp(ctx, p, button))
            return true;
    }
    return false;
}

bool Widget::onMouseMove(UIContext& ctx, const Vec2& p) {
    (void)ctx;
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->rect().contains(p) && (*it)->onMouseMove(ctx, p))
            return true;
    }
    return false;
}

bool Widget::onKeyDown(UIContext& ctx, int key) {
    (void)ctx;
    (void)key;
    return false;
}

bool Widget::onTextInput(UIContext& ctx, const std::string& text) {
    (void)ctx;
    (void)text;
    return false;
}

void UIContext::setRoot(std::unique_ptr<Widget> root) {
    m_root = std::move(root);
}

void UIContext::beginFrame() {
    m_drawList.clear();
    if (!m_root)
        return;
    UIRect r{0, 0, m_viewport.x, m_viewport.y};
    m_root->layout(*this, r);
    m_root->draw(*this);
}

void UIContext::endFrame() {
}

void UIContext::pushRect(const UIRect& r, const UIColor& c) {
    m_drawList.push_back({UIDrawCommand::Type::Rect, r, c, ""});
}

void UIContext::pushText(const UIRect& r, const UIColor& c, const std::string& text) {
    UIDrawCommand cmd;
    cmd.type = UIDrawCommand::Type::Text;
    cmd.rect = r;
    cmd.color = c;
    cmd.text = text;
    m_drawList.push_back(cmd);
}

bool UIContext::dispatchMouseDown(const Vec2& p, int button) {
    if (!m_root) return false;
    return m_root->onMouseDown(*this, p, button);
}

bool UIContext::dispatchMouseUp(const Vec2& p, int button) {
    if (!m_root) return false;
    return m_root->onMouseUp(*this, p, button);
}

bool UIContext::dispatchMouseMove(const Vec2& p) {
    if (!m_root) return false;
    return m_root->onMouseMove(*this, p);
}

bool UIContext::dispatchKeyDown(int key) {
    if (!m_focused) return false;
    return m_focused->onKeyDown(*this, key);
}

bool UIContext::dispatchTextInput(const std::string& text) {
    if (!m_focused) return false;
    return m_focused->onTextInput(*this, text);
}

} // namespace rex::ui
