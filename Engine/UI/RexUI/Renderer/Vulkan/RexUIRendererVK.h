#pragma once

#include "../IRenderBackend.h"
#include "../IRenderDevice.h"

namespace rex::ui::renderer::vulkan {

class RexUIRendererVK final : public IRenderBackend {
public:
    explicit RexUIRendererVK(IRenderDevice* device);

    bool beginFrame(const RenderFrameContext& context) override;
    bool submit(const runtime::render::DrawList& drawList) override;
    bool endFrame() override;

private:
    IRenderDevice* device_ = nullptr;
};

// TODO [RexUI-Renderer-Vulkan-001]:
// 책임: Vulkan 백엔드 구현 클래스 선언
// 요구사항:
//  - DrawList -> Vulkan command buffer 변환
//  - 파이프라인/디스크립터 관리 계약
//  - 멀티프레임 in-flight 동기화 규약
// 의존성:
//  - Renderer/IRenderBackend
//  - Renderer/IRenderDevice
// 구현 단계: Phase 3
// 성능 고려사항:
//  - descriptor/pipeline 캐시
//  - 버퍼 업데이트 동기화 최소화
// 테스트 전략:
//  - 백엔드 교체 호환 테스트
//  - long-run 안정성 테스트

} // namespace rex::ui::renderer::vulkan
