#pragma once

#include "IEditorPanel.h"

namespace rex::editor::panels {

class ModesPanel final : public IEditorPanel {
public:
    const char* id() const override { return "modes"; }
    const char* title() const override { return "Modes"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Panels-008]:
// 책임: Select/Landscape/Modeling 등 모드 전환 패널
// 요구사항:
//  - 모드별 툴셋 활성화
//  - 단축키 컨텍스트 연동
//  - 모드 전환 시 상태 동기화
// 의존성:
//  - Editor/Core/EditorApp
//  - Editor/Tools/*
// 구현 단계: Phase A
// 성능 고려사항:
//  - 모드 전환 시 불필요 재초기화 제거
//  - 툴 스위치 latency 최소화
// 테스트 전략:
//  - 모드 전환 회귀 테스트
//  - 컨텍스트 단축키 테스트

} // namespace rex::editor::panels

