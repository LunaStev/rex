#pragma once

#include <utility>

#include "World.h"

namespace rex::core::ecs {

template <typename... TComponents>
class Query {
public:
    explicit Query(World* world)
        : world_(world) {}

    template <typename Func>
    void each(Func&& func) {
        if (!world_) return;
        world_->template each<TComponents...>(std::forward<Func>(func));
    }

private:
    World* world_ = nullptr;
};

// TODO [Core-ECS-004]:
// 책임: 시스템 코드에서 재사용 가능한 Query 진입점 제공
// 요구사항:
//  - 컴포넌트 조합 기반 iteration
//  - const/non-const 조회 모드 분리
//  - 필터(태그/활성 상태) 확장 포인트
// 의존성:
//  - ECS/World
// 구현 단계: Phase C
// 성능 고려사항:
//  - 쿼리 컴파일 타임 특화
//  - 캐시 가능한 쿼리 플랜
// 테스트 전략:
//  - 조합별 query 결과 테스트
//  - 빈 월드/대형 월드 경계 테스트

} // namespace rex::core::ecs

