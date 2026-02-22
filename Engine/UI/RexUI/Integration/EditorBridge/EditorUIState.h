#pragma once

#include <string>

namespace rex::ui::integration::editorbridge {

struct EditorUIStatePaths {
    std::string selectedEntity = "editor.selection.entity";
    std::string activeViewport = "editor.viewport.active";
    std::string gizmoMode = "editor.gizmo.mode";
    std::string hierarchyFilter = "editor.hierarchy.filter";
    std::string undoAvailable = "editor.history.canUndo";
    std::string redoAvailable = "editor.history.canRedo";
};

// TODO [RexUI-Integration-001]:
// 책임: 에디터 UI 상태 path 계약 고정
// 요구사항:
//  - UIStateStore 경로 명세 일원화
//  - 모듈 간 문자열 상수 중복 제거
//  - 버전 업그레이드 호환 정책
// 의존성:
//  - Framework/State/UIStateStore
// 구현 단계: Phase C
// 성능 고려사항:
//  - path 문자열 할당 최소화
//  - 상수 접근 비용 무시 수준 유지
// 테스트 전략:
//  - path 충돌/오타 방지 테스트
//  - 버전 호환 mapping 테스트

} // namespace rex::ui::integration::editorbridge
