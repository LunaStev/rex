#include "DrawListBuilder.h"

#include <utility>

#include "../../Core/PaintContext.h"
#include "../Tree/WidgetTree.h"

namespace rex::ui::runtime::render {

namespace {
class DrawPaintContext final : public core::PaintContext {
public:
    explicit DrawPaintContext(DrawList* drawList)
        : drawList_(drawList) {}

    void drawRect(const core::Rect& rect, const core::Color& color) override {
        if (!drawList_) return;
        drawList_->push_back({DrawCommandType::Rect, rect, color, "", 0});
    }

    void drawBorder(const core::Rect& rect, const core::Color& color, float thickness) override {
        if (!drawList_) return;
        DrawCommand cmd;
        cmd.type = DrawCommandType::Border;
        cmd.rect = rect;
        cmd.rect.h = thickness;
        cmd.color = color;
        drawList_->push_back(std::move(cmd));
    }

    void drawText(const core::Rect& rect, const std::string& text, const core::Color& color) override {
        if (!drawList_) return;
        DrawCommand cmd;
        cmd.type = DrawCommandType::Text;
        cmd.rect = rect;
        cmd.color = color;
        cmd.text = text;
        drawList_->push_back(std::move(cmd));
    }

    void drawImage(const core::Rect& rect, std::uint64_t textureId, const core::Color& tint) override {
        if (!drawList_) return;
        DrawCommand cmd;
        cmd.type = DrawCommandType::Image;
        cmd.rect = rect;
        cmd.textureId = textureId;
        cmd.color = tint;
        drawList_->push_back(std::move(cmd));
    }

    void pushClip(const core::Rect& clipRect) override {
        if (!drawList_) return;
        drawList_->push_back({DrawCommandType::ClipPush, clipRect, {}, "", 0});
    }

    void popClip() override {
        if (!drawList_) return;
        drawList_->push_back({DrawCommandType::ClipPop, {}, {}, "", 0});
    }

private:
    DrawList* drawList_ = nullptr;
};
} // namespace

DrawList DrawListBuilder::build(const tree::WidgetTree& tree) const {
    DrawList drawList;
    const auto root = tree.root();
    if (!root) return drawList;

    DrawPaintContext ctx(&drawList);
    root->paint(ctx);
    return drawList;
}

} // namespace rex::ui::runtime::render
