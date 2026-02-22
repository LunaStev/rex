#pragma once

#include <memory>
#include <vector>

#include "../../../../Graphics/Shader.h"
#include "../../../RexUIFont.h"
#include "../IRenderBackend.h"
#include "../IRenderDevice.h"

namespace rex::ui::renderer::opengl {

class RexUIRendererGL final : public IRenderBackend {
public:
    explicit RexUIRendererGL(IRenderDevice* device);
    ~RexUIRendererGL() override;

    bool beginFrame(const RenderFrameContext& context) override;
    bool submit(const runtime::render::DrawList& drawList) override;
    bool endFrame() override;

private:
    bool ensureInitialized();
    void destroyGpuResources();
    void submitRect(const core::Rect& rect, const core::Color& color);
    void submitBorder(const core::Rect& rect, const core::Color& color, float thickness);
    void submitText(const core::Rect& rect, const std::string& text, const core::Color& color);
    void applyClipState();
    core::Rect intersectClips(const core::Rect& a, const core::Rect& b) const;

    IRenderDevice* device_ = nullptr;
    bool initialized_ = false;
    RenderFrameContext frame_{};

    std::unique_ptr<::rex::Shader> rectShader_{};
    std::unique_ptr<::rex::Shader> textShader_{};
    std::uint32_t rectVao_ = 0;
    std::uint32_t rectVbo_ = 0;
    std::uint32_t textVao_ = 0;
    std::uint32_t textVbo_ = 0;

    ::rex::ui::RexUIFont font_{};
    bool fontReady_ = false;
    std::vector<core::Rect> clipStack_{};
};

// TODO [RexUI-Renderer-OpenGL-001]:
// 책임: OpenGL 백엔드 구현 클래스 선언
// 요구사항:
//  - DrawList -> GL draw call 변환
//  - 클리핑/블렌딩/텍스트 아틀라스 처리
//  - 디버그 검증 레이어 훅
// 의존성:
//  - Renderer/IRenderBackend
//  - Renderer/IRenderDevice
// 구현 단계: Phase D
// 성능 고려사항:
//  - state change/VAO bind 최소화
//  - 텍스트/사각형 배치 드로우
// 테스트 전략:
//  - golden image 렌더 테스트
//  - GPU 리소스 누수 테스트

} // namespace rex::ui::renderer::opengl
