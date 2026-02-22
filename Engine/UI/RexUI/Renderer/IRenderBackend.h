#pragma once

#include "../Runtime/Render/DrawCommand.h"

namespace rex::ui::renderer {

struct RenderFrameContext {
    std::uint32_t viewportWidth = 0;
    std::uint32_t viewportHeight = 0;
    std::uint64_t frameIndex = 0;
};

class IRenderBackend {
public:
    virtual ~IRenderBackend() = default;

    virtual bool beginFrame(const RenderFrameContext& context) = 0;
    virtual bool submit(const runtime::render::DrawList& drawList) = 0;
    virtual bool endFrame() = 0;
};

// TODO [RexUI-Renderer-002]:
// 책임: DrawList 제출 백엔드 인터페이스 고정
// 요구사항:
//  - begin/submit/end 프레임 계약
//  - DrawCommand 타입 해석 공통 규약
//  - clip stack/text atlas 처리 계약
// 의존성:
//  - Runtime/Render/DrawCommand
// 구현 단계: Phase D
// 성능 고려사항:
//  - submit 경로 배치 최적화
//  - 상태 변경 최소화
// 테스트 전략:
//  - mock backend 제출 테스트
//  - 프레임 수명주기 위반 테스트

} // namespace rex::ui::renderer
