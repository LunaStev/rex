#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <vector>

#include "World.h"

namespace rex::core::ecs {

enum class SystemPhase : std::uint8_t {
    PreUpdate = 0,
    Update,
    PostUpdate,
    Count
};

class SystemScheduler {
public:
    using SystemFn = std::function<void(World&, float)>;

    void registerSystem(SystemPhase phase, SystemFn system) {
        systems_[static_cast<std::size_t>(phase)].push_back(std::move(system));
    }

    void run(World& world, float dt) {
        for (std::size_t i = 0; i < static_cast<std::size_t>(SystemPhase::Count); ++i) {
            for (auto& system : systems_[i]) {
                if (system) {
                    system(world, dt);
                }
            }
        }
    }

private:
    std::array<std::vector<SystemFn>, static_cast<std::size_t>(SystemPhase::Count)> systems_{};
};

// TODO [Core-ECS-005]:
// 책임: ECS 시스템 실행 순서와 페이즈 관리
// 요구사항:
//  - phase 기반 시스템 등록
//  - 월드 갱신 시 결정적 실행 순서
//  - 의존성 기반 정렬 확장 지점
// 의존성:
//  - ECS/World
// 구현 단계: Phase C
// 성능 고려사항:
//  - 시스템 dispatch 오버헤드 최소화
//  - 프레임별 재정렬 비용 제어
// 테스트 전략:
//  - 실행 순서 고정 테스트
//  - 등록/해제 회귀 테스트

} // namespace rex::core::ecs

