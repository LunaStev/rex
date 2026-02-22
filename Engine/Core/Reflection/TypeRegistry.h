#pragma once

#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "PropertyMetadata.h"

namespace rex::core::reflection {

class TypeRegistry {
public:
    static TypeRegistry& instance() {
        static TypeRegistry inst;
        return inst;
    }

    template <typename T>
    void registerType(TypeMetadata metadata) {
        byIndex_[std::type_index(typeid(T))] = metadata;
        byName_[metadata.name] = std::move(metadata);
    }

    template <typename T>
    const TypeMetadata* find() const {
        auto it = byIndex_.find(std::type_index(typeid(T)));
        if (it == byIndex_.end()) return nullptr;
        return &it->second;
    }

    const TypeMetadata* findByName(const std::string& name) const {
        auto it = byName_.find(name);
        if (it == byName_.end()) return nullptr;
        return &it->second;
    }

private:
    std::unordered_map<std::type_index, TypeMetadata> byIndex_;
    std::unordered_map<std::string, TypeMetadata> byName_;
};

// TODO [Core-Reflection-002]:
// 책임: 런타임 타입 메타 레지스트리 제공
// 요구사항:
//  - 타입 등록/조회 API
//  - type_index 및 name 기반 조회
//  - 모듈 경계에서 메타 병합 가능
// 의존성:
//  - Reflection/PropertyMetadata
// 구현 단계: Phase D
// 성능 고려사항:
//  - 조회 O(1)
//  - 초기화 시 정적 등록 순서 안정성
// 테스트 전략:
//  - 등록/조회 테스트
//  - 중복 타입 충돌 테스트

} // namespace rex::core::reflection

