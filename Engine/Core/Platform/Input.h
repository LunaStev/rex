#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace rex::core::platform {

enum class InputDevice {
    Keyboard,
    Mouse,
    Gamepad
};

enum class InputType {
    ButtonDown,
    ButtonUp,
    Axis,
    Text
};

struct InputEvent {
    InputDevice device = InputDevice::Keyboard;
    InputType type = InputType::ButtonDown;
    std::int32_t code = 0;
    float value = 0.0f;
    std::string text;
};

// TODO [Core-Platform-004]:
// 책임: 플랫폼 입력 이벤트 공통 포맷 정의
// 요구사항:
//  - 키보드/마우스/게임패드 통합
//  - 버튼/축/텍스트 입력 분리
//  - 상위 입력 매핑 레이어와 독립
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 입력 이벤트 복사 최소화
//  - burst 입력 버퍼 최적화
// 테스트 전략:
//  - 디바이스별 이벤트 생성 테스트
//  - 텍스트 입력 경계 테스트

} // namespace rex::core::platform

