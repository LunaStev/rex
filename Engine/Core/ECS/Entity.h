#pragma once

#include <cstdint>

namespace rex::core::ecs {

using EntityId = std::uint32_t;
constexpr EntityId kInvalidEntity = 0xffffffffu;

// TODO [Core-ECS-001]:
// 책임: 엔티티 식별자 정책 정의
// 요구사항:
//  - 재사용 가능한 ID/세대(generation) 확장 포인트
//  - invalid sentinel 명시
//  - 직렬화 가능한 기본 타입 유지
// 의존성:
//  - 없음
// 구현 단계: Phase C
// 성능 고려사항:
//  - ID 발급/해제 O(1)
//  - 대량 엔티티 생성 시 락 경합 최소화
// 테스트 전략:
//  - 대량 생성/삭제 충돌 테스트
//  - invalid ID 방어 테스트

} // namespace rex::core::ecs

