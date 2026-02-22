#pragma once

#include <cstdint>
#include <string>

#include "IEditorPanel.h"

namespace rex::editor::panels {

class ContentBrowserPanel final : public IEditorPanel {
public:
    const char* id() const override { return "content_browser"; }
    const char* title() const override { return "Content Browser"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;

    void setPathFilter(const std::string& filter);
    void setTagFilter(const std::string& filter);

private:
    std::string pathFilter_;
    std::string tagFilter_;
    std::uint64_t dockPanelId_ = 0;
};

// TODO [Editor-Panels-004]:
// 책임: 애셋 탐색/필터/드래그앤드롭 패널
// 요구사항:
//  - AssetRegistry 기반 목록 표시
//  - 폴더/태그/타입 필터 지원
//  - 씬으로 drag&drop 인스턴스화 연동
// 의존성:
//  - Editor/Core/EditorApp
//  - Editor/Asset/AssetRegistry
// 구현 단계: Phase A
// 성능 고려사항:
//  - 비동기 썸네일 로드
//  - 대량 애셋 스크롤 가상화
// 테스트 전략:
//  - 필터 조합 테스트
//  - drag&drop 동작 테스트

} // namespace rex::editor::panels
