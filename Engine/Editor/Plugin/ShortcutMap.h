#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

namespace rex::editor::plugin {

struct ShortcutChord {
    std::int32_t keyCode = 0;
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
};

class ShortcutMap {
public:
    void bind(const std::string& context, const ShortcutChord& chord, const std::string& commandId);
    void unbind(const std::string& context, const ShortcutChord& chord);

    std::optional<std::string> resolve(const std::string& context, const ShortcutChord& chord) const;

private:
    std::unordered_map<std::string, std::unordered_map<std::uint64_t, std::string>> bindings_;
};

// TODO [Editor-Plugin-005]:
// 책임: 컨텍스트 기반 단축키 매핑 제공
// 요구사항:
//  - context + chord -> command id 해석
//  - 충돌 해결 정책
//  - 런타임 리바인딩
// 의존성:
//  - Editor/Plugin/CommandRegistry
// 구현 단계: Phase C
// 성능 고려사항:
//  - resolve O(1) 목표
//  - 컨텍스트 전환 비용 최소화
// 테스트 전략:
//  - 컨텍스트별 단축키 해석 테스트
//  - 충돌 케이스 테스트

} // namespace rex::editor::plugin

