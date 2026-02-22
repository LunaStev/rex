#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../../Core/Geometry.h"
#include "../../Core/StyleValue.h"

namespace rex::ui::runtime::render {

enum class DrawCommandType {
    Rect,
    Border,
    Text,
    Image,
    ClipPush,
    ClipPop
};

struct DrawCommand {
    DrawCommandType type = DrawCommandType::Rect;
    core::Rect rect{};
    core::Color color{};
    std::string text;
    std::uint64_t textureId = 0;
    float thickness = 0.0f;
};

using DrawList = std::vector<DrawCommand>;

// TODO [RexUI-Runtime-Render-001]:
// 책임: 백엔드 중립 DrawCommand 포맷 정의
// 요구사항:
//  - 기본 프리미티브(Rect/Text/Image/Clip)
//  - 텍스트/이미지 리소스 핸들 표현
//  - 배치 가능한 명령 구조
// 의존성:
//  - Core/Geometry
//  - Core/StyleValue
// 구현 단계: Phase D
// 성능 고려사항:
//  - 명령 버퍼 연속 메모리 유지
//  - 문자열/텍스처 핸들 복사 최소화
// 테스트 전략:
//  - 명령 시퀀스 직렬화 테스트
//  - 클리핑 push/pop 균형 테스트

} // namespace rex::ui::runtime::render
