#include "RexUIRendererVK.h"

namespace rex::ui::renderer::vulkan {

// TODO [RexUI-Renderer-Vulkan-002]:
// 책임: Vulkan 백엔드 메서드 구현
// 요구사항:
//  - beginFrame/submit/endFrame 구현
//  - DrawCommand 타입별 그래픽스 파이프라인 매핑
//  - text atlas/clip stack 처리
// 의존성:
//  - Renderer/Vulkan/RexUIRendererVK.h
//  - Runtime/Render/DrawCommand
// 구현 단계: Phase 3
// 성능 고려사항:
//  - command buffer 재사용
//  - 메모리 배리어 최소화
// 테스트 전략:
//  - 렌더 일치성 테스트(GL 대비)
//  - GPU validation layer 테스트

} // namespace rex::ui::renderer::vulkan
