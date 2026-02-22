#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

namespace rex::ui::core {

enum class ThemeMode {
    Dark,
    Light
};

enum class WidgetStateFlag : std::uint32_t {
    None = 0,
    Hovered = 1u << 0,
    Pressed = 1u << 1,
    Focused = 1u << 2,
    Disabled = 1u << 3,
    Selected = 1u << 4,
    Error = 1u << 5
};

inline WidgetStateFlag operator|(WidgetStateFlag a, WidgetStateFlag b) {
    return static_cast<WidgetStateFlag>(
        static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
}

inline WidgetStateFlag operator&(WidgetStateFlag a, WidgetStateFlag b) {
    return static_cast<WidgetStateFlag>(
        static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));
}

inline WidgetStateFlag& operator|=(WidgetStateFlag& a, WidgetStateFlag b) {
    a = a | b;
    return a;
}

inline bool hasFlag(WidgetStateFlag value, WidgetStateFlag flag) {
    return static_cast<std::uint32_t>(value & flag) != 0u;
}

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

struct StyleToken {
    std::string key;
};

using StyleValue = std::variant<float, std::int32_t, bool, std::string, Color>;

class ThemeSet {
public:
    bool has(const std::string& token) const;
    std::optional<StyleValue> find(const std::string& token) const;
    void set(const std::string& token, const StyleValue& value);

private:
    std::unordered_map<std::string, StyleValue> values_;
};

class StyleResolver {
public:
    virtual ~StyleResolver() = default;

    virtual std::optional<StyleValue> resolve(
        const std::string& styleClass,
        WidgetStateFlag state,
        const std::string& token) const = 0;
};

// TODO [RexUI-Core-003]:
// 책임: 테마 토큰/스타일 값 해석 파이프라인 정의
// 요구사항:
//  - 전역 테마 + 로컬 오버라이드 우선순위
//  - 상태별 셀렉터(Normal/Hover/Focused...)
//  - 다크/라이트 전환 시 invalidate 전략
// 의존성:
//  - 없음
// 구현 단계: Phase B
// 성능 고려사항:
//  - 스타일 조회 O(1) 근접
//  - 토큰 해시 충돌 최소화
// 테스트 전략:
//  - 테마 전환 snapshot 테스트
//  - 상태 조합별 스타일 해석 테스트

} // namespace rex::ui::core
