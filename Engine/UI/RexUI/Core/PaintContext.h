#pragma once

#include <cstdint>
#include <string>

#include "Geometry.h"
#include "StyleValue.h"

namespace rex::ui::core {

class PaintContext {
public:
    virtual ~PaintContext() = default;

    virtual void drawRect(const Rect& rect, const Color& color) = 0;
    virtual void drawBorder(const Rect& rect, const Color& color, float thickness) = 0;
    virtual void drawText(const Rect& rect, const std::string& text, const Color& color) = 0;
    virtual void drawImage(const Rect& rect, std::uint64_t textureId, const Color& tint) = 0;

    virtual void pushClip(const Rect& clipRect) = 0;
    virtual void popClip() = 0;
};

// TODO [RexUI-Core-006]:
// 책임: 위젯 paint 단계의 백엔드 중립 출력 인터페이스 제공
// 요구사항:
//  - 기본 드로우 명령(drawRect/drawText/drawImage)
//  - clip push/pop 수명주기 보장
//  - 런타임 DrawListBuilder와의 연동
// 의존성:
//  - Core/Geometry
//  - Core/StyleValue
// 구현 단계: Phase D
// 성능 고려사항:
//  - hot path 가상 호출 최소화
//  - 문자열/리소스 핸들 복사 비용 제어
// 테스트 전략:
//  - 명령 발행 순서 테스트
//  - clip 균형(push/pop) 테스트

} // namespace rex::ui::core

