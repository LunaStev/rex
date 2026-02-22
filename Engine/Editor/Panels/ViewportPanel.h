#pragma once

#include "IEditorPanel.h"

namespace rex::editor::gizmo {
class TransformGizmo;
}

namespace rex::editor::panels {

class ViewportPanel final : public IEditorPanel {
public:
    const char* id() const override { return "viewport"; }
    const char* title() const override { return "Viewport"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;

    void setDebugOverlay(bool enabled);
    bool debugOverlayEnabled() const;

private:
    bool debugOverlay_ = true;
    bool showSelectionOutline_ = true;
    bool showPostProcess_ = true;
};

// TODO [Editor-Panels-001]:
// 책임: 실제 엔진 렌더링을 표시하는 Scene Viewport 패널
// 요구사항:
//  - gizmo/선택/카메라 입력 연동
//  - 렌더 모드(wireframe/lighting-only) 토글
//  - 디버그 오버레이(FPS/frametime) 표시
// 의존성:
//  - Editor/Core/EditorApp
//  - Editor/Gizmo/TransformGizmo
// 구현 단계: Phase A
// 성능 고려사항:
//  - viewport resize 시 재할당 최소화
//  - 오버레이 렌더 비용 제어
// 테스트 전략:
//  - 카메라/선택 입력 테스트
//  - 렌더 모드 전환 테스트

} // namespace rex::editor::panels

