#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "../../UI/RexUI/Framework/Commands/UICommand.h"

namespace rex::editor::plugin {

class CommandRegistry {
public:
    using Factory = std::function<std::shared_ptr<ui::framework::commands::UICommand>()>;

    bool registerCommand(const std::string& commandId, Factory factory);
    void unregisterCommand(const std::string& commandId);

    std::shared_ptr<ui::framework::commands::UICommand> create(const std::string& commandId) const;
    bool contains(const std::string& commandId) const;

private:
    std::unordered_map<std::string, Factory> factories_;
};

// TODO [Editor-Plugin-004]:
// 책임: 에디터 명령 팩토리 등록소 제공
// 요구사항:
//  - command id 기반 생성
//  - 단축키/메뉴 바인딩 가능한 명령 키 구조
//  - 모듈 언로드 시 정리
// 의존성:
//  - UI/RexUI/Framework/Commands/UICommand
// 구현 단계: Phase C
// 성능 고려사항:
//  - command 생성 경로 최적화
//  - ID 조회 O(1)
// 테스트 전략:
//  - 등록/생성/해제 테스트
//  - 중복 id 충돌 테스트

} // namespace rex::editor::plugin

