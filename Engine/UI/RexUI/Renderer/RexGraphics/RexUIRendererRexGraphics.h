#pragma once

#include "../IRenderBackend.h"
#include "../IRenderDevice.h"
#include "RexGraphicsAPI.h"

namespace rex::ui::renderer::rexgraphics {

class RexUIRendererRexGraphics final : public IRenderBackend {
public:
    RexUIRendererRexGraphics(IRenderDevice* resourceDevice, IRexGraphicsDevice* graphicsDevice);

    bool beginFrame(const RenderFrameContext& context) override;
    bool submit(const runtime::render::DrawList& drawList) override;
    bool endFrame() override;

private:
    IRenderDevice* resourceDevice_ = nullptr;
    IRexGraphicsDevice* graphicsDevice_ = nullptr;
    RexGraphicsCommandBuffer commandBuffer_{};
};

// TODO [RexUI-Renderer-RexGraphics-001]:
// 책임: DrawList를 RexGraphics 명령으로 변환/제출
// 요구사항:
//  - DrawCommand -> RexGraphicsOp 매핑
//  - begin/submit/end 경로 구현
//  - 텍스트/이미지 핸들 전달
// 의존성:
//  - Renderer/IRenderBackend
//  - Renderer/RexGraphics/RexGraphicsAPI
// 구현 단계: Phase D
// 성능 고려사항:
//  - commandBuffer 재사용
//  - 변환 중 동적 할당 최소화
// 테스트 전략:
//  - 명령 매핑 회귀 테스트
//  - 프레임 반복 안정성 테스트

} // namespace rex::ui::renderer::rexgraphics

