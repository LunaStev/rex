#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ElementId.h"
#include "Event.h"
#include "Geometry.h"
#include "PaintContext.h"
#include "StyleValue.h"

namespace rex::ui::core {

class Widget : public std::enable_shared_from_this<Widget> {
public:
    virtual ~Widget() = default;

    ElementId id() const;
    void setId(ElementId id);

    Widget* parent() const;
    const std::vector<std::shared_ptr<Widget>>& children() const;

    void addChild(const std::shared_ptr<Widget>& child);
    void removeChild(ElementId childId);
    void clearChildren();

    virtual Size measure(const LayoutConstraints& constraints, const EventContext& ctx) const;
    virtual void arrange(const Rect& finalRect);
    virtual void paint(PaintContext& ctx) const;
    virtual bool handleEvent(EventContext& ctx, RoutedEvent& event);

    const Rect& arrangedRect() const;
    void setStyleClass(const std::string& styleClass);
    const std::string& styleClass() const;

    void resolveStyle(const StyleResolver& resolver, WidgetStateFlag state);
    std::optional<StyleValue> styleValue(const std::string& token) const;

protected:
    virtual void onStyleResolved();

private:
    ElementId id_ = kInvalidElementId;
    Widget* parent_ = nullptr;
    std::vector<std::shared_ptr<Widget>> children_;
    Rect arrangedRect_{};
    std::string styleClass_;
    std::unordered_map<std::string, StyleValue> resolvedStyleValues_;
};

// TODO [RexUI-Core-005]:
// 책임: Widget 기본 수명주기 인터페이스 확정
// 요구사항:
//  - measure/arrange/paint 계약 고정
//  - 이벤트 훅(handleEvent) 표준화
//  - StyleResolver 연동 지점 정의
// 의존성:
//  - Geometry
//  - Event
//  - StyleValue
// 구현 단계: Phase A
// 성능 고려사항:
//  - 위젯 트리 순회 시 메모리 지역성
//  - 불필요한 style resolve 방지
// 테스트 전략:
//  - 기본 위젯 수명주기 호출 순서 테스트
//  - 트리 추가/삭제 무결성 테스트

} // namespace rex::ui::core
