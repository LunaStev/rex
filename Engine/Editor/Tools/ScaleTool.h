#pragma once

#include "IEditorTool.h"

namespace rex::editor::tools {

class ScaleTool final : public IEditorTool {
public:
    const char* id() const override { return "scale_tool"; }
    bool onEnter(core::EditorApp& app) override;
    void onExit(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Tools-004]:
// 책임: Scale Gizmo 기반 스케일 툴
// 요구사항:
//  - 축별/균일 스케일 조절
//  - scale snap 반영
//  - 음수/0 스케일 방지 정책
// 의존성:
//  - Editor/Gizmo/TransformGizmo
// 구현 단계: Phase B
// 성능 고려사항:
//  - 프레임당 반복 계산 최적화
//  - 대량 대상 batch 적용 경로
// 테스트 전략:
//  - 스케일 스냅 테스트
//  - 경계값(0/음수) 방어 테스트

} // namespace rex::editor::tools

