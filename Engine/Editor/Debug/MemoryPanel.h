#pragma once

#include "../Panels/IEditorPanel.h"

namespace rex::editor::debug {

class MemoryPanel final : public panels::IEditorPanel {
public:
    const char* id() const override { return "memory_panel"; }
    const char* title() const override { return "Memory"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Debug-003]:
// 책임: 메모리 사용량/할당기 통계 패널
// 요구사항:
//  - allocator별 사용량 표시
//  - 프레임 할당량 추세 시각화
//  - 누수 의심 경고 표시 포인트
// 의존성:
//  - Editor/Panels/IEditorPanel
//  - Core/Memory/*
// 구현 단계: Phase C
// 성능 고려사항:
//  - 통계 수집 오버헤드 최소화
//  - 샘플링 주기 제어
// 테스트 전략:
//  - 통계 수집 정확성 테스트
//  - 대량 할당 시 안정성 테스트

} // namespace rex::editor::debug

