#pragma once

#include <string>

#include "../../Core/Widget.h"

namespace rex::ui::widgets::basic {

class TextWidget : public core::Widget {
public:
    void setText(const std::string& text);
    const std::string& text() const;

    void setWrapping(bool enabled);
    bool wrapping() const;

    core::Size measure(const core::LayoutConstraints& constraints, const core::EventContext& ctx) const override;
    void paint(core::PaintContext& ctx) const override;

private:
    std::string text_;
    bool wrapping_ = false;
};

// TODO [RexUI-Widgets-Basic-003]:
// 책임: 텍스트 위젯 측정/표시 인터페이스 선언
// 요구사항:
//  - 단일/멀티라인 측정 계약
//  - 줄바꿈 옵션(wrapping)
//  - 텍스트 스타일 토큰 연동
// 의존성:
//  - Core/Widget
//  - Core/Geometry
// 구현 단계: Phase B
// 성능 고려사항:
//  - 텍스트 측정 캐시
//  - 글리프 아틀라스 조회 최소화
// 테스트 전략:
//  - 다양한 문자열 측정 테스트
//  - wrapping 경계 테스트

} // namespace rex::ui::widgets::basic
