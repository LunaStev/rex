#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace rex::ui::core {
class Widget;
}

namespace rex::ui::framework::plugins {

class WidgetRegistry {
public:
    using Factory = std::function<std::shared_ptr<core::Widget>()>;

    bool registerFactory(const std::string& widgetType, Factory factory);
    void unregisterFactory(const std::string& widgetType);

    std::shared_ptr<core::Widget> create(const std::string& widgetType) const;
    bool contains(const std::string& widgetType) const;

private:
    std::unordered_map<std::string, Factory> factories_;
};

// TODO [RexUI-Framework-Plugins-001]:
// 책임: 위젯 팩토리 등록/생성 레지스트리
// 요구사항:
//  - 문자열 타입 -> 팩토리 매핑
//  - 런타임 등록/해제 지원
//  - 중복 등록 충돌 정책
// 의존성:
//  - Core/Widget
// 구현 단계: Phase D
// 성능 고려사항:
//  - create 경로 해시 조회 최적화
//  - 플러그인 언로드 시 dangling 방지
// 테스트 전략:
//  - 등록/해제/생성 테스트
//  - 중복 키 정책 테스트

} // namespace rex::ui::framework::plugins
