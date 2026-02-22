#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "CommandRegistry.h"
#include "EditorPanelRegistry.h"
#include "EditorToolRegistry.h"
#include "IEditorModule.h"
#include "InspectorWidgetRegistry.h"
#include "ShortcutMap.h"

namespace rex::editor::plugin {

class EditorModuleManager {
public:
    bool load(const std::shared_ptr<IEditorModule>& module);
    bool unload(const std::string& moduleName);
    bool isLoaded(const std::string& moduleName) const;

    EditorPanelRegistry& panelRegistry();
    EditorToolRegistry& toolRegistry();
    InspectorWidgetRegistry& inspectorRegistry();
    CommandRegistry& commandRegistry();
    ShortcutMap& shortcutMap();

private:
    EditorPanelRegistry panels_{};
    EditorToolRegistry tools_{};
    InspectorWidgetRegistry inspectors_{};
    CommandRegistry commands_{};
    ShortcutMap shortcuts_{};
    std::unordered_map<std::string, std::shared_ptr<IEditorModule>> loaded_;
};

// TODO [Editor-Plugin-007]:
// 책임: 에디터 모듈 로드/언로드 및 레지스트리 집계
// 요구사항:
//  - 모듈 lifecycle 관리
//  - 등록된 확장 포인트 레지스트리 제공
//  - 모듈 단위 실패 격리
// 의존성:
//  - Editor/Plugin/*
// 구현 단계: Phase C
// 성능 고려사항:
//  - 모듈 조회 O(1)
//  - 대량 모듈 로드 시 초기화 순서 최적화
// 테스트 전략:
//  - 모듈 load/unload 테스트
//  - registry 누수 테스트

} // namespace rex::editor::plugin

