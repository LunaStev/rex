#pragma once

#include "IEditorTool.h"

namespace rex::editor::tools {

class RotateTool final : public IEditorTool {
public:
    const char* id() const override { return "rotate_tool"; }
    bool onEnter(core::EditorApp& app) override;
    void onExit(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Tools-003]:
// 책임: Rotate Gizmo 기반 회전 툴
// 요구사항:
//  - local/world 회전
//  - 회전 스냅(각도 단위) 반영
//  - 다중 선택 회전 피벗 규칙 제공
// 의존성:
//  - Editor/Gizmo/TransformGizmo
// 구현 단계: Phase B
// 성능 고려사항:
//  - 쿼터니언 누적 오차 최소화
//  - 실시간 프리뷰 비용 제어
// 테스트 전략:
//  - 회전 스냅 테스트
//  - undo/redo 회전 일관성 테스트

} // namespace rex::editor::tools

