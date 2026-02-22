#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace rex::editor::plugin {

class InspectorWidgetRegistry {
public:
    using Factory = std::function<void*()>;

    bool registerInspectorWidget(const std::string& propertyType, Factory factory);
    void unregisterInspectorWidget(const std::string& propertyType);

    Factory find(const std::string& propertyType) const;
    bool contains(const std::string& propertyType) const;

private:
    std::unordered_map<std::string, Factory> factories_;
};

// TODO [Editor-Plugin-003]:
// 책임: 커스텀 인스펙터 위젯 팩토리 레지스트리 제공
// 요구사항:
//  - property type 기반 editor widget 매핑
//  - 기본/커스텀 우선순위 정책
//  - 타입 미스매치 fallback 규칙
// 의존성:
//  - Core/Reflection
// 구현 단계: Phase C
// 성능 고려사항:
//  - 조회 O(1)
//  - 위젯 생성 캐시 가능성 확보
// 테스트 전략:
//  - 타입별 매핑 테스트
//  - fallback 동작 테스트

} // namespace rex::editor::plugin

