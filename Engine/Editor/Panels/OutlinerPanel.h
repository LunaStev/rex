#pragma once

#include <cstdint>
#include <string>

#include "IEditorPanel.h"

namespace rex::editor::panels {

class OutlinerPanel final : public IEditorPanel {
public:
    const char* id() const override { return "outliner"; }
    const char* title() const override { return "World Outliner"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;

    void setSearchFilter(const std::string& filter);
    const std::string& searchFilter() const;

private:
    std::string searchFilter_;
    std::uint64_t dockPanelId_ = 0;
};

// TODO [Editor-Panels-002]:
// 책임: 엔티티 계층 트리/검색/선택 패널
// 요구사항:
//  - virtualized hierarchy 표시
//  - 검색 필터/선택 동기화
//  - 100k 엔티티 대응 성능 경로
// 의존성:
//  - Editor/Core/EditorApp
// 구현 단계: Phase A
// 성능 고려사항:
//  - 가시 영역만 realize
//  - 검색 debounce 적용
// 테스트 전략:
//  - 대규모 트리 성능 테스트
//  - 선택 동기화 테스트

} // namespace rex::editor::panels
