#pragma once

#include "IEditorTool.h"

namespace rex::editor::tools {

class MoveTool final : public IEditorTool {
public:
    const char* id() const override { return "move_tool"; }
    bool onEnter(core::EditorApp& app) override;
    void onExit(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Tools-002]:
// 책임: Translate Gizmo 기반 이동 툴
// 요구사항:
//  - local/world 이동
//  - grid snap 반영
//  - 다중 선택 batch 이동
// 의존성:
//  - Editor/Gizmo/TransformGizmo
// 구현 단계: Phase B
// 성능 고려사항:
//  - 드래그 중 command batching
//  - 프리뷰 업데이트 경량화
// 테스트 전략:
//  - 이동 정확성 테스트
//  - 스냅 이동 테스트

} // namespace rex::editor::tools

