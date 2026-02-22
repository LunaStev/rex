#pragma once

#include "../Panels/IEditorPanel.h"

namespace rex::editor::debug {

class ProfilerPanel final : public panels::IEditorPanel {
public:
    const char* id() const override { return "profiler"; }
    const char* title() const override { return "Profiler"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Debug-001]:
// 책임: CPU/GPU frametime 그래프 패널
// 요구사항:
//  - 실시간 FPS/frametime 표시
//  - 샘플 히스토리 시각화
//  - 엔진 프로파일 훅과 연동
// 의존성:
//  - Editor/Panels/IEditorPanel
//  - Core/Diagnostics/ProfilerHooks
// 구현 단계: Phase C
// 성능 고려사항:
//  - 샘플 버퍼 고정 크기 유지
//  - 그래프 렌더 비용 제어
// 테스트 전략:
//  - 샘플 수집/표시 테스트
//  - 장시간 실행 안정성 테스트

} // namespace rex::editor::debug

