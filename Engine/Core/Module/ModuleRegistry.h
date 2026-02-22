#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "ModuleABI.h"

namespace rex::core::module {

class ModuleRegistry {
public:
    bool registerModule(const std::shared_ptr<IModule>& module) {
        if (!module) return false;
        const std::string name = module->descriptor().name;
        if (name.empty()) return false;
        return modules_.emplace(name, module).second;
    }

    void unregisterModule(const std::string& name) {
        modules_.erase(name);
    }

    std::shared_ptr<IModule> find(const std::string& name) const {
        auto it = modules_.find(name);
        if (it == modules_.end()) return {};
        return it->second;
    }

    bool contains(const std::string& name) const {
        return modules_.find(name) != modules_.end();
    }

private:
    std::unordered_map<std::string, std::shared_ptr<IModule>> modules_;
};

// TODO [Core-Module-002]:
// 책임: 모듈 등록/조회 레지스트리
// 요구사항:
//  - name 기반 등록/해제/조회
//  - 중복 모듈 방지
//  - 로더와 분리된 저장소 역할
// 의존성:
//  - Module/ModuleABI
// 구현 단계: Phase D
// 성능 고려사항:
//  - lookup O(1)
//  - 등록/해제 경로 잠금 전략
// 테스트 전략:
//  - 등록/중복/해제 테스트
//  - 조회 실패 경로 테스트

} // namespace rex::core::module

