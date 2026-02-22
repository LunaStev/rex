#pragma once

#include "../RexMath.h"

namespace rex::core::math {

using ::rex::Mat4;
using ::rex::Quat;
using ::rex::Vec2;
using ::rex::Vec3;
using ::rex::Vec4;

// TODO [Core-Math-001]:
// 책임: Core 수학 타입 네임스페이스 정규화
// 요구사항:
//  - 기존 RexMath 타입과 호환
//  - 점진적 이관 시 include 경로 안정화
//  - SIMD 확장 포인트 유지
// 의존성:
//  - Core/RexMath.h
// 구현 단계: Phase A
// 성능 고려사항:
//  - 래퍼 계층 오버헤드 0
//  - 인라이닝 친화 구조 유지
// 테스트 전략:
//  - 타입 호환 컴파일 테스트
//  - 수학 함수 회귀 테스트

} // namespace rex::core::math

