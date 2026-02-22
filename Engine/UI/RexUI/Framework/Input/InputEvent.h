#pragma once

#include <cstdint>
#include <string>

namespace rex::ui::framework::input {

enum class InputDeviceType {
    Keyboard,
    Mouse,
    Gamepad
};

enum class InputEventType {
    ButtonDown,
    ButtonUp,
    Axis,
    Text
};

struct InputEvent {
    InputDeviceType device = InputDeviceType::Mouse;
    InputEventType type = InputEventType::ButtonDown;
    std::int32_t code = 0;
    float value = 0.0f;
    std::string text;
};

// TODO [RexUI-Framework-Input-001]:
// 책임: 물리 입력 이벤트 공통 포맷 정의
// 요구사항:
//  - 키/마우스/게임패드 통합 이벤트 구조
//  - 버튼/축/텍스트 이벤트 분리
//  - 플랫폼 코드 -> 공통 코드 매핑 규약
// 의존성:
//  - 없음
// 구현 단계: Phase B
// 성능 고려사항:
//  - 이벤트 객체 복사 최소화
//  - 입력 burst 처리 버퍼링
// 테스트 전략:
//  - 디바이스별 매핑 테스트
//  - 텍스트 입력/IME 경계 테스트

} // namespace rex::ui::framework::input
