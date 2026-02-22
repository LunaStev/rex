#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../../Core/Geometry.h"
#include "../../Core/StyleValue.h"

namespace rex::ui::renderer::rexgraphics {

enum class RexGraphicsOp : std::uint8_t {
    BeginPass,
    EndPass,
    DrawRect,
    DrawBorder,
    DrawText,
    DrawImage,
    PushClip,
    PopClip
};

struct RexGraphicsCommand {
    RexGraphicsOp op = RexGraphicsOp::DrawRect;
    core::Rect rect{};
    core::Color color{};
    std::uint64_t textureId = 0;
    float thickness = 0.0f;
    std::string text;
};

using RexGraphicsCommandBuffer = std::vector<RexGraphicsCommand>;

struct RexGraphicsFrameContext {
    std::uint32_t viewportWidth = 0;
    std::uint32_t viewportHeight = 0;
    std::uint64_t frameIndex = 0;
};

class IRexGraphicsDevice {
public:
    virtual ~IRexGraphicsDevice() = default;

    virtual bool beginFrame(const RexGraphicsFrameContext& context) = 0;
    virtual bool submit(const RexGraphicsCommandBuffer& commands) = 0;
    virtual bool endFrame() = 0;
};

class NullRexGraphicsDevice final : public IRexGraphicsDevice {
public:
    bool beginFrame(const RexGraphicsFrameContext& context) override {
        (void)context;
        submittedCommandCount_ = 0;
        return true;
    }

    bool submit(const RexGraphicsCommandBuffer& commands) override {
        submittedCommandCount_ += commands.size();
        return true;
    }

    bool endFrame() override {
        return true;
    }

    std::size_t submittedCommandCount() const {
        return submittedCommandCount_;
    }

private:
    std::size_t submittedCommandCount_ = 0;
};

// TODO [RexGraphics-API-001]:
// 책임: RexGraphics 커맨드 스트림/디바이스 추상화 제공
// 요구사항:
//  - 백엔드 중립 명령(opcode + payload) 정의
//  - begin/submit/end 프레임 계약
//  - Null 디바이스로 테스트 경로 보장
// 의존성:
//  - Core/Geometry
//  - Core/StyleValue
// 구현 단계: Phase D
// 성능 고려사항:
//  - 커맨드 버퍼 연속 메모리 유지
//  - 문자열 payload 최소화/아틀라스 핸들 전환
// 테스트 전략:
//  - 명령 변환 정확성 테스트
//  - 프레임 수명주기 테스트

} // namespace rex::ui::renderer::rexgraphics

