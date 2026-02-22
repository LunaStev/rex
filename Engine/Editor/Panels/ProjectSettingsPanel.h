#pragma once

#include "IEditorPanel.h"

namespace rex::editor::panels {

class ProjectSettingsPanel final : public IEditorPanel {
public:
    const char* id() const override { return "project_settings"; }
    const char* title() const override { return "Project Settings"; }

    bool onAttach(core::EditorApp& app) override;
    void onDetach(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Panels-007]:
// 책임: 프로젝트/엔진 설정 편집 패널
// 요구사항:
//  - 설정 항목 카테고리화
//  - 변경사항 트랜잭션 기록
//  - 저장/되돌리기 워크플로우
// 의존성:
//  - Editor/Core/EditorApp
//  - Editor/Core/EditorCommandBus
// 구현 단계: Phase A
// 성능 고려사항:
//  - 설정 검색 인덱스 최적화
//  - 대량 설정 항목 가시화 최적화
// 테스트 전략:
//  - 설정 저장/복원 테스트
//  - undo/redo 테스트

} // namespace rex::editor::panels

