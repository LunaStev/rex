#pragma once
#include "../Core/RexMath.h"
#include <memory>
#include <string>
#include <vector>

namespace rex::ui {

struct UIRect {
    float x = 0;
    float y = 0;
    float w = 0;
    float h = 0;

    bool contains(const Vec2& p) const {
        return p.x >= x && p.x <= x + w && p.y >= y && p.y <= y + h;
    }
};

struct UIColor {
    float r = 1;
    float g = 1;
    float b = 1;
    float a = 1;
};

struct UIStyle {
    UIColor bg{0, 0, 0, 0};
    UIColor border{0, 0, 0, 0};
    UIColor text{1, 1, 1, 1};
    float padding = 0;
    float margin = 0;
    float borderWidth = 0;
};

enum class UILayout {
    Absolute,
    Row,
    Column
};

enum class UIAlign {
    Start,
    Center,
    End,
    Stretch
};

struct UIDrawCommand {
    enum class Type { Rect, Text } type = Type::Rect;
    UIRect rect;
    UIColor color;
    std::string text;
};

class UIContext;

enum class UIKey {
    Backspace = 8,
    Enter = 13,
    DeleteKey = 127,
    Left = 256,
    Right = 257
};

class Widget {
public:
    Widget();
    virtual ~Widget();

    void addChild(std::unique_ptr<Widget> child);
    const std::vector<std::unique_ptr<Widget>>& children() const { return m_children; }

    void setLayout(UILayout l) { m_layout = l; }
    void setAlign(UIAlign a) { m_align = a; }
    void setStyle(const UIStyle& s) { m_style = s; }
    void setPreferredSize(const Vec2& s) { m_preferred = s; }
    void setFlex(float f) { m_flex = f; }

    const UIRect& rect() const { return m_rect; }
    void setRect(const UIRect& r) { m_rect = r; }

    virtual Vec2 measure(UIContext& ctx, const Vec2& available);
    virtual void layout(UIContext& ctx, const UIRect& r);
    virtual void draw(UIContext& ctx);

    virtual bool onMouseDown(UIContext& ctx, const Vec2& p, int button);
    virtual bool onMouseUp(UIContext& ctx, const Vec2& p, int button);
    virtual bool onMouseMove(UIContext& ctx, const Vec2& p);
    virtual bool onKeyDown(UIContext& ctx, int key);
    virtual bool onTextInput(UIContext& ctx, const std::string& text);

protected:
    UILayout m_layout = UILayout::Absolute;
    UIAlign  m_align = UIAlign::Start;
    UIStyle  m_style{};
    Vec2     m_preferred{0, 0};
    float    m_flex = 0.0f;
    UIRect   m_rect{};
    std::vector<std::unique_ptr<Widget>> m_children;
};

class UIContext {
public:
    void setRoot(std::unique_ptr<Widget> root);
    Widget* root() const { return m_root.get(); }

    void setViewportSize(const Vec2& size) { m_viewport = size; }
    const Vec2& viewportSize() const { return m_viewport; }

    void beginFrame();
    void endFrame();

    void pushRect(const UIRect& r, const UIColor& c);
    void pushText(const UIRect& r, const UIColor& c, const std::string& text);
    const std::vector<UIDrawCommand>& drawList() const { return m_drawList; }

    bool dispatchMouseDown(const Vec2& p, int button);
    bool dispatchMouseUp(const Vec2& p, int button);
    bool dispatchMouseMove(const Vec2& p);
    bool dispatchKeyDown(int key);
    bool dispatchTextInput(const std::string& text);

    void setFocused(Widget* w) { m_focused = w; }
    Widget* focused() const { return m_focused; }

private:
    std::unique_ptr<Widget> m_root;
    Vec2 m_viewport{0, 0};
    std::vector<UIDrawCommand> m_drawList;
    Widget* m_focused = nullptr;
};

} // namespace rex::ui
