#pragma once

#include "IEditorPanel.h"

namespace rex::editor::panels {

class ToolbarPanel final : public IEditorPanel {
public:
    const char* id() const override { return "toolbar"; }
    const char* title() const override { return "Toolbar"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Panels-005]:
// 책임: 공용 명령(저장/실행/모드전환) 노출 패널
// 요구사항:
//  - command registry와 연동된 버튼/메뉴
//  - 단축키 힌트 표시
//  - 프로젝트 실행/빌드 진입점 제공
// 의존성:
//  - Editor/Core/EditorApp
//  - Editor/Plugin/CommandRegistry
// 구현 단계: Phase A
// 성능 고려사항:
//  - 프레임당 재빌드 최소화
//  - 아이콘/텍스트 캐시
// 테스트 전략:
//  - 명령 디스패치 테스트
//  - 단축키 트리거 테스트

} // namespace rex::editor::panels

