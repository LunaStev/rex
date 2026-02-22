#pragma once

#include "../Panels/IEditorPanel.h"

namespace rex::editor::debug {

class LogPanel final : public panels::IEditorPanel {
public:
    const char* id() const override { return "log_panel"; }
    const char* title() const override { return "Log"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Debug-002]:
// 책임: 로그 필터링/탐색 패널
// 요구사항:
//  - 레벨/카테고리 필터
//  - 검색/스크롤 유지
//  - 로그 source jump 확장 포인트
// 의존성:
//  - Editor/Panels/IEditorPanel
//  - Core/Diagnostics/Logger
// 구현 단계: Phase C
// 성능 고려사항:
//  - 대량 로그 렌더 가상화
//  - 필터 재계산 최적화
// 테스트 전략:
//  - 필터 정확성 테스트
//  - 로그 버퍼 상한 테스트

} // namespace rex::editor::debug

