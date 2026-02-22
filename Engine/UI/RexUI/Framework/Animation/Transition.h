#pragma once

#include <string>

#include "Easing.h"

namespace rex::ui::framework::animation {

struct TransitionSpec {
    std::string stateFrom;
    std::string stateTo;
    float duration = 0.15f;
    EasingType easing = EasingType::EaseOut;
};

class TransitionController {
public:
    void setTransition(const std::string& widgetType, const TransitionSpec& spec);
    bool start(const std::string& widgetType, const std::string& from, const std::string& to);
    void tick(float dt);
};

// TODO [RexUI-Framework-Animation-003]:
// 책임: 상태 전이 기반 트랜지션 제어
// 요구사항:
//  - Hover/Focus/PanelOpen 전이 사양
//  - 위젯 타입별 전이 정책 등록
//  - 동시 전이 충돌 해결 규칙
// 의존성:
//  - Animation/Easing
//  - Animation/Timeline
// 구현 단계: Phase E
// 성능 고려사항:
//  - 활성 전이 목록 관리 비용 제어
//  - 매우 짧은 전이의 오버헤드 최소화
// 테스트 전략:
//  - 상태 전이 경로 테스트
//  - 전이 중단/교체 테스트

} // namespace rex::ui::framework::animation
