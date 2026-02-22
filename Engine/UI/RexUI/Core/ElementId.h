#pragma once

#include <cstdint>

namespace rex::ui::core {

using ElementId = std::uint64_t;
constexpr ElementId kInvalidElementId = 0;

class ElementIdGenerator {
public:
    ElementIdGenerator() = default;

    ElementId next();
    void reset(ElementId seed = 1);

private:
    ElementId nextId_ = 1;
};

// TODO [RexUI-Core-001]:
// 책임: 위젯/엘리먼트 고유 식별자 정책 정의
// 요구사항:
//  - 프레임 간 안정적 ID 생성/재사용 정책
//  - Diff 엔진과 호환되는 키 전략
//  - 직렬화 가능한 ID 포맷
// 의존성:
//  - 없음
// 구현 단계: Phase A
// 성능 고려사항:
//  - ID 생성 O(1)
//  - 동시성 환경에서 lock 최소화
// 테스트 전략:
//  - 대량 생성 충돌 테스트
//  - 리셋/복원 일관성 테스트

} // namespace rex::ui::core
