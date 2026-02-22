#pragma once

#include <string>

#include "IEditorPanel.h"

namespace rex::editor::panels {

class OutputLogPanel final : public IEditorPanel {
public:
    const char* id() const override { return "output_log"; }
    const char* title() const override { return "Output Log"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;

    void setCategoryFilter(const std::string& category);
    void setTextFilter(const std::string& text);

private:
    std::string categoryFilter_;
    std::string textFilter_;
};

// TODO [Editor-Panels-006]:
// 책임: 로그 출력/필터/콘솔 패널
// 요구사항:
//  - 레벨/카테고리/텍스트 필터
//  - 실시간 로그 스트리밍
//  - 콘솔 명령 입력(선택) 확장 포인트
// 의존성:
//  - Editor/Core/EditorApp
//  - Core/Diagnostics/Logger
// 구현 단계: Phase A
// 성능 고려사항:
//  - 대량 로그 버퍼링 정책
//  - UI 갱신 빈도 제어
// 테스트 전략:
//  - 필터 정확성 테스트
//  - 로그 폭주 시 안정성 테스트

} // namespace rex::editor::panels

