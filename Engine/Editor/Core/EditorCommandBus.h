#pragma once

#include <memory>

#include "../../UI/RexUI/Framework/Commands/CommandBus.h"
#include "../../UI/RexUI/Framework/Commands/UndoRedoStack.h"

namespace rex::editor::core {

class EditorCommandBus {
public:
    EditorCommandBus();

    bool execute(const std::shared_ptr<ui::framework::commands::UICommand>& command);
    bool undo();
    bool redo();

    bool canUndo() const;
    bool canRedo() const;

    ui::framework::commands::CommandBus& rawBus();
    const ui::framework::commands::CommandBus& rawBus() const;

private:
    ui::framework::commands::UICommandContext context_{};
    ui::framework::commands::UndoRedoStack undoRedo_{};
    ui::framework::commands::CommandBus bus_{&undoRedo_};
};

// TODO [Editor-Core-003]:
// 책임: 에디터 변경 커맨드 실행/히스토리 단일 경로 제공
// 요구사항:
//  - execute/undo/redo API
//  - UI 직접 모델 변경 금지 정책 강제
//  - canUndo/canRedo 상태 노출
// 의존성:
//  - UI/RexUI/Framework/Commands/*
// 구현 단계: Phase A
// 성능 고려사항:
//  - 고빈도 명령 처리 오버헤드 최소화
//  - 히스토리 메모리 상한 정책
// 테스트 전략:
//  - 실행/취소/재실행 시퀀스 테스트
//  - 실패 명령 롤백 테스트

} // namespace rex::editor::core

