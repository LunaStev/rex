#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "../Panels/IEditorPanel.h"

namespace rex::editor::plugin {

class EditorPanelRegistry {
public:
    using Factory = std::function<std::shared_ptr<panels::IEditorPanel>()>;

    bool registerPanel(const std::string& panelId, Factory factory);
    void unregisterPanel(const std::string& panelId);

    std::shared_ptr<panels::IEditorPanel> create(const std::string& panelId) const;
    bool contains(const std::string& panelId) const;

private:
    std::unordered_map<std::string, Factory> factories_;
};

// TODO [Editor-Plugin-001]:
// 책임: 패널 팩토리 등록/생성 레지스트리 제공
// 요구사항:
//  - panel id 기반 create
//  - 중복 id 등록 방지 정책
//  - 모듈 언로드 시 해제 경로 보장
// 의존성:
//  - Editor/Panels/IEditorPanel
// 구현 단계: Phase C
// 성능 고려사항:
//  - create lookup O(1)
//  - 팩토리 캡처 메모리 관리
// 테스트 전략:
//  - 등록/해제/생성 테스트
//  - 중복 등록 테스트

} // namespace rex::editor::plugin

