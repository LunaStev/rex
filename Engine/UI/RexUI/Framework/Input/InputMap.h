#pragma once

#include <string>

#include "InputEvent.h"

namespace rex::ui::framework::input {

class InputMap {
public:
    void bindAction(const std::string& context, const std::string& actionName, const InputEvent& trigger);
    void unbindAction(const std::string& context, const std::string& actionName);

    bool resolveAction(const std::string& context, const InputEvent& event, std::string& outActionName) const;
};

// TODO [RexUI-Framework-Input-002]:
// 책임: 논리 입력 액션 매핑 레이어
// 요구사항:
//  - context 기반 action binding
//  - 충돌 binding 우선순위 정책
//  - 런타임 리바인딩 지원
// 의존성:
//  - Input/InputEvent
// 구현 단계: Phase B
// 성능 고려사항:
//  - resolveAction O(1)~O(logN)
//  - 컨텍스트 스위치 비용 최소화
// 테스트 전략:
//  - 액션 충돌/우선순위 테스트
//  - 리바인딩 회귀 테스트

} // namespace rex::ui::framework::input
