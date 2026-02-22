#pragma once

#include <string>

#include "../../Core/ElementId.h"

namespace rex::ui::runtime::diff {

enum class PatchOpType {
    Add,
    Remove,
    Replace,
    UpdateProperty,
    Move
};

struct PatchOp {
    PatchOpType type = PatchOpType::UpdateProperty;
    core::ElementId target = core::kInvalidElementId;
    core::ElementId parent = core::kInvalidElementId;
    std::string property;
    std::string payload;
};

// TODO [RexUI-Runtime-Diff-001]:
// 책임: 트리 변경 패치 연산 포맷 정의
// 요구사항:
//  - Add/Remove/Replace/Move/PropertyUpdate 지원
//  - 패치 직렬화 가능 구조
//  - 적용 순서 규약
// 의존성:
//  - Core/ElementId
// 구현 단계: Phase D
// 성능 고려사항:
//  - 패치 벡터 메모리 풀링
//  - 문자열 payload 비용 최소화
// 테스트 전략:
//  - 패치 순서/역적용 테스트
//  - 잘못된 대상 id 예외 처리 테스트

} // namespace rex::ui::runtime::diff
