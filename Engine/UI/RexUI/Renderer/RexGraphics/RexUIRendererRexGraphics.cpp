#include "RexUIRendererRexGraphics.h"

namespace rex::ui::renderer::rexgraphics {

RexUIRendererRexGraphics::RexUIRendererRexGraphics(
    IRenderDevice* resourceDevice,
    IRexGraphicsDevice* graphicsDevice)
    : resourceDevice_(resourceDevice)
    , graphicsDevice_(graphicsDevice) {}

bool RexUIRendererRexGraphics::beginFrame(const RenderFrameContext& context) {
    if (!graphicsDevice_) return false;
    commandBuffer_.clear();
    return graphicsDevice_->beginFrame({
        context.viewportWidth,
        context.viewportHeight,
        context.frameIndex
    });
}

bool RexUIRendererRexGraphics::submit(const runtime::render::DrawList& drawList) {
    if (!graphicsDevice_) return false;
    commandBuffer_.reserve(commandBuffer_.size() + drawList.size());

    for (const auto& cmd : drawList) {
        RexGraphicsCommand out{};
        out.rect = cmd.rect;
        out.color = cmd.color;
        out.textureId = cmd.textureId;
        out.text = cmd.text;

        switch (cmd.type) {
            case runtime::render::DrawCommandType::Rect:
                out.op = RexGraphicsOp::DrawRect;
                break;
            case runtime::render::DrawCommandType::Border:
                out.op = RexGraphicsOp::DrawBorder;
                out.thickness = cmd.thickness;
                break;
            case runtime::render::DrawCommandType::Text:
                out.op = RexGraphicsOp::DrawText;
                break;
            case runtime::render::DrawCommandType::Image:
                out.op = RexGraphicsOp::DrawImage;
                break;
            case runtime::render::DrawCommandType::ClipPush:
                out.op = RexGraphicsOp::PushClip;
                break;
            case runtime::render::DrawCommandType::ClipPop:
                out.op = RexGraphicsOp::PopClip;
                break;
        }

        commandBuffer_.push_back(std::move(out));
    }

    (void)resourceDevice_;
    return graphicsDevice_->submit(commandBuffer_);
}

bool RexUIRendererRexGraphics::endFrame() {
    if (!graphicsDevice_) return false;
    return graphicsDevice_->endFrame();
}

} // namespace rex::ui::renderer::rexgraphics
