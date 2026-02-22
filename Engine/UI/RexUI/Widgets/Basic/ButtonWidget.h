#pragma once

#include <functional>
#include <string>

#include "../../Core/Widget.h"

namespace rex::ui::widgets::basic {

class ButtonWidget : public core::Widget {
public:
    using ClickHandler = std::function<void()>;

    void setText(const std::string& text);
    const std::string& text() const;

    void setOnClick(ClickHandler handler);
    core::Size measure(const core::LayoutConstraints& constraints, const core::EventContext& ctx) const override;
    void paint(core::PaintContext& ctx) const override;
    bool handleEvent(core::EventContext& ctx, core::RoutedEvent& event) override;

private:
    std::string text_;
    ClickHandler onClick_{};
    bool pressed_ = false;
};

// TODO [RexUI-Widgets-Basic-002]:
// 책임: 버튼 위젯 상호작용 인터페이스 선언
// 요구사항:
//  - hover/press/release 상태 처리
//  - onClick 콜백 훅
//  - 상태 기반 스타일 전이 연동
// 의존성:
//  - Core/Widget
//  - Core/Event
// 구현 단계: Phase B
// 성능 고려사항:
//  - 이벤트 처리 분기 최소화
//  - 스타일 상태 전이 캐시
// 테스트 전략:
//  - 클릭/취소/드래그아웃 테스트
//  - 상태 전이 시각 회귀 테스트

} // namespace rex::ui::widgets::basic
