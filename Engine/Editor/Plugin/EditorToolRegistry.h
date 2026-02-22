#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "../Tools/IEditorTool.h"

namespace rex::editor::plugin {

class EditorToolRegistry {
public:
    using Factory = std::function<std::shared_ptr<tools::IEditorTool>()>;

    bool registerTool(const std::string& toolId, Factory factory);
    void unregisterTool(const std::string& toolId);

    std::shared_ptr<tools::IEditorTool> create(const std::string& toolId) const;
    bool contains(const std::string& toolId) const;

private:
    std::unordered_map<std::string, Factory> factories_;
};

// TODO [Editor-Plugin-002]:
// 책임: 툴 팩토리 등록/생성 레지스트리 제공
// 요구사항:
//  - tool id 기반 생성
//  - 모드/컨텍스트 확장 가능한 키 구조
//  - 중복 id 충돌 정책
// 의존성:
//  - Editor/Tools/IEditorTool
// 구현 단계: Phase C
// 성능 고려사항:
//  - lookup O(1)
//  - 툴 생성 오버헤드 최소화
// 테스트 전략:
//  - 등록/생성/해제 테스트
//  - 모듈 언로드 테스트

} // namespace rex::editor::plugin

