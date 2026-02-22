#pragma once

#include <string>

#include "InputEvent.h"

namespace rex::ui::core {
class Widget;
}

namespace rex::ui::framework::input {

class InputMap;

class InputRouter {
public:
    explicit InputRouter(InputMap* map);

    void setActiveContext(const std::string& context);
    const std::string& activeContext() const;

    bool routeToWidgetTree(core::Widget* root, const InputEvent& event);
    bool routeToAction(const InputEvent& event, std::string& outActionName);
};

// TODO [RexUI-Framework-Input-003]:
// 책임: 입력 이벤트 라우팅(위젯/액션) 분기
// 요구사항:
//  - 활성 입력 컨텍스트 기반 라우팅
//  - 위젯 이벤트와 액션 이벤트 병행 처리
//  - 포커스/캡처 대상 우선 라우팅
// 의존성:
//  - Input/InputMap
//  - Core/Widget
// 구현 단계: Phase B
// 성능 고려사항:
//  - 입력 프레임당 분기 최소화
//  - 액션 resolve 캐시
// 테스트 전략:
//  - 컨텍스트 전환 테스트
//  - 포커스/캡처 우선순위 테스트

} // namespace rex::ui::framework::input
