#pragma once

#include <memory>

#include "IRenderBackend.h"
#include "IRenderDevice.h"
#include "RenderBackendType.h"
#include "RexGraphics/RexGraphicsAPI.h"

namespace rex::ui::renderer {

std::unique_ptr<IRenderBackend> createRenderBackend(
    RenderBackendType type,
    IRenderDevice* renderDevice,
    rexgraphics::IRexGraphicsDevice* rexGraphicsDevice = nullptr);

// TODO [RexUI-Renderer-005]:
// 책임: 런타임 백엔드 생성 팩토리 제공
// 요구사항:
//  - OpenGL/Vulkan/RexGraphics 생성 분기
//  - 의존 디바이스 주입
//  - 알 수 없는 타입 방어
// 의존성:
//  - Renderer/IRenderBackend
//  - Renderer/RenderBackendType
// 구현 단계: Phase D
// 성능 고려사항:
//  - 백엔드 생성은 시작 시 1회 수행
//  - 런타임 분기 최소화
// 테스트 전략:
//  - 타입별 생성 성공 테스트
//  - nullptr 디바이스 실패 테스트

} // namespace rex::ui::renderer

