#pragma once

#include "../../Core/Geometry.h"

namespace rex::ui::runtime::layout {

class ConstraintSolver {
public:
    core::Size solve(const core::LayoutConstraints& constraints, const core::Size& desired) const;
};

// TODO [RexUI-Runtime-Layout-001]:
// 책임: 위젯 size 제약 해결 규칙 정의
// 요구사항:
//  - min/max 제약 반영
//  - stretch/fixed 정책 연결 포인트
//  - 잘못된 제약 입력 정규화
// 의존성:
//  - Core/Geometry
// 구현 단계: Phase B
// 성능 고려사항:
//  - 대량 위젯 제약 연산 O(1)
//  - 분기 예측 친화적 구현
// 테스트 전략:
//  - min/max 경계 테스트
//  - 잘못된 제약 정규화 테스트

} // namespace rex::ui::runtime::layout
