#include "RexUIRendererGL.h"

namespace rex::ui::renderer::opengl {

// TODO [RexUI-Renderer-OpenGL-002]:
// 책임: OpenGL 백엔드 메서드 구현
// 요구사항:
//  - beginFrame/submit/endFrame 구현
//  - DrawCommand 타입별 렌더 파이프라인 매핑
//  - clip stack 및 text atlas cache 처리
// 의존성:
//  - Renderer/OpenGL/RexUIRendererGL.h
//  - Runtime/Render/DrawCommand
// 구현 단계: Phase D
// 성능 고려사항:
//  - draw call batching
//  - CPU->GPU 업로드 최소화
// 테스트 전략:
//  - 프레임 렌더 정합성 테스트
//  - 성능 벤치마크(명령 수별 FPS)

RexUIRendererGL::RexUIRendererGL(IRenderDevice* device)
    : device_(device) {}

bool RexUIRendererGL::beginFrame(const RenderFrameContext& context) {
    (void)context;
    return true;
}

bool RexUIRendererGL::submit(const runtime::render::DrawList& drawList) {
    (void)drawList;
    return device_ != nullptr;
}

bool RexUIRendererGL::endFrame() {
    return true;
}

} // namespace rex::ui::renderer::opengl
