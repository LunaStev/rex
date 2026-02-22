#pragma once

#include <cstdint>
#include <string>

#include "Geometry.h"

namespace rex::ui::core {

class Widget;

enum class EventType {
    PointerDown,
    PointerUp,
    PointerMove,
    PointerWheel,
    KeyDown,
    KeyUp,
    TextInput,
    FocusIn,
    FocusOut,
    Command
};

enum class EventPhase {
    Capture,
    Target,
    Bubble
};

struct RoutedEvent {
    EventType type = EventType::PointerMove;
    EventPhase phase = EventPhase::Target;
    bool handled = false;
    bool stopPropagation = false;
    bool stopImmediate = false;
};

struct PointerEvent : public RoutedEvent {
    Point position{};
    float wheelDelta = 0.0f;
    std::uint32_t button = 0;
};

struct KeyboardEvent : public RoutedEvent {
    std::int32_t keyCode = 0;
    bool repeat = false;
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
};

struct TextInputEvent : public RoutedEvent {
    std::string text;
};

struct EventContext {
    Widget* root = nullptr;
    Widget* target = nullptr;
    Widget* focused = nullptr;
    Widget* captured = nullptr;
};

// TODO [RexUI-Core-004]:
// 책임: 캡처링/버블링 기반 이벤트 모델 표준화
// 요구사항:
//  - 이벤트 위상(Capture/Target/Bubble) 명확화
//  - stopPropagation/handled 규약 고정
//  - 포인터 캡처/포커스 전이 계약 정의
// 의존성:
//  - Geometry
//  - Widget(forward)
// 구현 단계: Phase B
// 성능 고려사항:
//  - 이벤트 라우팅 경로 캐시
//  - 고빈도 포인터 이벤트 할당 최소화
// 테스트 전략:
//  - 이벤트 순서 결정성 테스트
//  - 캡처/버블 경계 케이스 테스트

} // namespace rex::ui::core
