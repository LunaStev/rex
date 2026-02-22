#pragma once

#include "IEditorTool.h"

namespace rex::editor::tools {

class SelectionTool final : public IEditorTool {
public:
    const char* id() const override { return "selection_tool"; }
    bool onEnter(core::EditorApp& app) override;
    void onExit(core::EditorApp& app) override;
    void onTick(core::EditorApp& app, float dt) override;
};

// TODO [Editor-Tools-001]:
// 책임: 엔티티 선택/다중선택 툴
// 요구사항:
//  - 클릭/박스/추가선택 동작
//  - Viewport와 SelectionManager 동기화
//  - 필터(선택 가능 타입) 지원 확장
// 의존성:
//  - Editor/Core/SelectionManager
// 구현 단계: Phase B
// 성능 고려사항:
//  - 레이캐스트 후보 축소 최적화
//  - 대량 선택 이벤트 처리 제어
// 테스트 전략:
//  - 단일/다중/박스 선택 테스트
//  - 선택 취소 시나리오 테스트

} // namespace rex::editor::tools

