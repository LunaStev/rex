#pragma once

#include <cstdint>
#include <string>

#include "IEditorPanel.h"

namespace rex::editor::panels {

class DetailsPanel final : public IEditorPanel {
public:
    const char* id() const override { return "details"; }
    const char* title() const override { return "Details"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;

    void setShowAdvanced(bool enabled);
    bool showAdvanced() const;

private:
    bool showAdvanced_ = false;
    std::uint64_t dockPanelId_ = 0;
};

// TODO [Editor-Panels-003]:
// 책임: Reflection 기반 Property Grid 패널
// 요구사항:
//  - 선택 대상 속성 자동 생성
//  - metadata(range/step/tooltip) 반영
//  - 값 커밋 시 CommandBus 사용
// 의존성:
//  - Editor/Core/EditorApp
//  - Core/Reflection/*
// 구현 단계: Phase A
// 성능 고려사항:
//  - 필드 rebuild 최소화
//  - batch property edit 경로 최적화
// 테스트 전략:
//  - 속성 편집/undo 테스트
//  - metadata 반영 테스트

} // namespace rex::editor::panels
