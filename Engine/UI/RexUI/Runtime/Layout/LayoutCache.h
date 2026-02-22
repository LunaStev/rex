#pragma once

#include <cstdint>
#include <optional>

#include "../../Core/ElementId.h"
#include "../../Core/Geometry.h"

namespace rex::ui::runtime::layout {

struct LayoutCacheKey {
    core::ElementId elementId = core::kInvalidElementId;
    std::uint64_t styleHash = 0;
    std::uint64_t contentHash = 0;
    core::LayoutConstraints constraints{};
};

struct LayoutCacheEntry {
    LayoutCacheKey key{};
    core::LayoutResult result{};
    std::uint64_t frameStamp = 0;
};

class LayoutCache {
public:
    void beginFrame(std::uint64_t frameStamp);
    std::optional<core::LayoutResult> find(const LayoutCacheKey& key) const;
    void store(const LayoutCacheEntry& entry);
    void clear();
};

// TODO [RexUI-Runtime-Layout-002]:
// 책임: 레이아웃 계산 결과 캐싱
// 요구사항:
//  - key(style/content/constraint) 기반 조회
//  - 프레임 단위 수명 관리
//  - 캐시 무효화 정책 정의
// 의존성:
//  - Core/ElementId
//  - Core/Geometry
// 구현 단계: Phase B
// 성능 고려사항:
//  - 높은 hit ratio 유지
//  - 캐시 메모리 상한/eviction 정책
// 테스트 전략:
//  - hit/miss 정확성 테스트
//  - 무효화 시 stale 데이터 방지 테스트

} // namespace rex::ui::runtime::layout
