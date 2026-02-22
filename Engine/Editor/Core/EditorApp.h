#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../../UI/RexUI/App/RexUIEngine.h"
#include "../../UI/RexUI/Framework/Docking/DockManager.h"
#include "../Plugin/EditorModuleManager.h"
#include "EditorCommandBus.h"
#include "EditorSession.h"
#include "EditorStateStore.h"
#include "LayoutService.h"
#include "SelectionManager.h"
#include "TransactionSystem.h"

namespace rex::editor::core {

class EditorApp {
public:
    explicit EditorApp(ui::app::RexUIEngine* uiEngine);

    bool initialize(const EditorWorkspace& workspace);
    void shutdown();

    bool tick(float dt, std::uint64_t frameIndex);

    EditorStateStore& stateStore();
    EditorCommandBus& commandBus();
    SelectionManager& selection();
    TransactionSystem& transactions();
    ui::framework::docking::DockManager& dockManager();
    plugin::EditorModuleManager& moduleManager();

private:
    ui::app::RexUIEngine* uiEngine_ = nullptr;
    EditorSession session_{};
    EditorStateStore stateStore_{};
    EditorCommandBus commandBus_{};
    SelectionManager selection_{};
    TransactionSystem transactions_{};
    ui::framework::docking::DockManager dockManager_{};
    LayoutService layoutService_{};
    plugin::EditorModuleManager moduleManager_{};
};

// TODO [Editor-Core-009]:
// 책임: 에디터 애플리케이션 최상위 오케스트레이터
// 요구사항:
//  - initialize/tick/shutdown 수명주기
//  - 상태/선택/명령/트랜잭션/모듈 관리자 결합
//  - UI 엔진 프레임 구동 연동
// 의존성:
//  - Editor/Core/*
//  - Editor/Plugin/EditorModuleManager
//  - UI/RexUI/App/RexUIEngine
// 구현 단계: Phase A
// 성능 고려사항:
//  - tick 경로에서 불필요 동적 할당 제거
//  - 모듈 업데이트 비용 계측 가능성 확보
// 테스트 전략:
//  - lifecycle end-to-end 테스트
//  - 세션 재시작 안정성 테스트

} // namespace rex::editor::core

