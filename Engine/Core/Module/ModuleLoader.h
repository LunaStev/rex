#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "ModuleRegistry.h"

namespace rex::core::module {

class ModuleLoader {
public:
    explicit ModuleLoader(ModuleRegistry* registry)
        : registry_(registry) {}

    bool load(const std::string& name) {
        if (!registry_) return false;
        auto module = registry_->find(name);
        if (!module) return false;
        if (loaded_[name]) return true;

        for (const auto& dep : module->descriptor().dependencies) {
            if (!load(dep)) return false;
        }

        if (!module->startup()) return false;
        loaded_[name] = true;
        return true;
    }

    bool unload(const std::string& name) {
        if (!registry_) return false;
        auto module = registry_->find(name);
        if (!module) return false;
        if (!loaded_[name]) return true;
        module->shutdown();
        loaded_[name] = false;
        return true;
    }

    bool isLoaded(const std::string& name) const {
        auto it = loaded_.find(name);
        return it != loaded_.end() && it->second;
    }

private:
    ModuleRegistry* registry_ = nullptr;
    std::unordered_map<std::string, bool> loaded_;
};

// TODO [Core-Module-003]:
// 책임: 모듈 로드/언로드 및 의존성 해석
// 요구사항:
//  - 의존 모듈 선로드
//  - startup/shutdown 호출
//  - 로드 상태 추적
// 의존성:
//  - Module/ModuleRegistry
// 구현 단계: Phase D
// 성능 고려사항:
//  - 중복 로딩 방지
//  - 의존 그래프 순회 비용 최소화
// 테스트 전략:
//  - 의존 체인 로드 테스트
//  - startup 실패 롤백 테스트

} // namespace rex::core::module

