#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace rex::core::event {

enum class EngineEventType : std::uint16_t {
    None = 0,
    EngineInit,
    EngineShutdown,
    SceneLoaded,
    SceneUnloaded,
    ModuleLoaded,
    ModuleUnloaded,
    ResourceLoaded,
    ResourceUnloaded
};

using EngineEventPayload = std::variant<std::monostate, bool, std::int64_t, double, std::string>;

struct EngineEvent {
    EngineEventType type = EngineEventType::None;
    EngineEventPayload payload{};
};

// TODO [Core-Event-001]:
// 책임: 엔진 전역 이벤트 타입과 페이로드 표준화
// 요구사항:
//  - lifecycle/module/resource 이벤트 타입
//  - 확장 가능한 payload 형식
//  - UI 이벤트와 분리된 전역 이벤트 계약
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - payload 복사 최소화
//  - 이벤트 객체 크기 제어
// 테스트 전략:
//  - 타입별 이벤트 라우팅 테스트
//  - payload 직렬화 테스트

} // namespace rex::core::event

