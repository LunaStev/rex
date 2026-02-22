#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "../../Core/ElementId.h"
#include "../../Core/Geometry.h"

namespace rex::ui::runtime::tree {

struct ElementRecord {
    core::ElementId id = core::kInvalidElementId;
    core::ElementId parent = core::kInvalidElementId;
    core::Rect rect{};
    bool visible = true;
    std::uint64_t revision = 0;
};

class ElementTree {
public:
    void clear();

    bool has(core::ElementId id) const;
    ElementRecord* find(core::ElementId id);
    const ElementRecord* find(core::ElementId id) const;

    void upsert(const ElementRecord& record);
    void erase(core::ElementId id);

    const std::vector<ElementRecord>& records() const;

private:
    std::vector<ElementRecord> records_;
    std::unordered_map<core::ElementId, std::size_t> indexById_;
};

// TODO [RexUI-Runtime-Tree-001]:
// 책임: 런타임 엘리먼트 상태(사각형/가시성/리비전) 보관
// 요구사항:
//  - id 기반 조회/갱신/삭제
//  - parent 관계 무결성 유지
//  - revision 증가 규칙
// 의존성:
//  - Core/ElementId
//  - Core/Geometry
// 구현 단계: Phase A
// 성능 고려사항:
//  - 조회 O(1) 인덱싱
//  - 프레임별 갱신 메모리 재할당 최소화
// 테스트 전략:
//  - insert/update/delete 무결성 테스트
//  - parent-child 일관성 테스트

} // namespace rex::ui::runtime::tree
