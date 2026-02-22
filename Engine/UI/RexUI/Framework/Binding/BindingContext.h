#pragma once

#include <cstdint>
#include <unordered_map>

#include "BindingExpression.h"

namespace rex::ui::core {
class Widget;
}

namespace rex::ui::framework::state {
class UIStateStore;
}

namespace rex::ui::framework::binding {

class BindingContext {
public:
    BindingContext(state::UIStateStore* stateStore, core::Widget* rootWidget);

    state::UIStateStore* stateStore() const;
    core::Widget* rootWidget() const;

private:
    state::UIStateStore* stateStore_ = nullptr;
    core::Widget* rootWidget_ = nullptr;
};

class BindingEngine {
public:
    using BindingId = std::uint64_t;

    BindingId bind(core::Widget* widget, const BindingExpression& expression, BindingContext& context);
    void unbind(BindingId id, BindingContext& context);
    void evaluate(BindingContext& context);
    void flush(BindingContext& context);

private:
    struct BindingRecord {
        core::Widget* widget = nullptr;
        BindingExpression expression{};
        std::uint64_t stateSubscriptionId = 0;
    };

    BindingId nextBindingId_ = 1;
    std::unordered_map<BindingId, BindingRecord> bindings_;
};

// TODO [RexUI-Framework-Binding-002]:
// 책임: 위젯 속성-상태 저장소 바인딩 실행기
// 요구사항:
//  - bind/unbind 라이프사이클
//  - 초기 evaluate + 변경 flush
//  - TwoWay 입력을 CommandBus 경유로 위임
// 의존성:
//  - BindingExpression
//  - State/UIStateStore
//  - Core/Widget
// 구현 단계: Phase C
// 성능 고려사항:
//  - dirty binding만 평가
//  - 대량 바인딩 인덱싱 최적화
// 테스트 전략:
//  - OneWay/TwoWay 모드 테스트
//  - 바인딩 해제 누수 테스트

} // namespace rex::ui::framework::binding
