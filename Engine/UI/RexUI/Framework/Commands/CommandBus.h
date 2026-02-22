#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "UICommand.h"

namespace rex::ui::framework::commands {

class UndoRedoStack;

class CommandBus {
public:
    using ListenerId = std::uint64_t;
    using Listener = std::function<void(const UICommand&)>;

    explicit CommandBus(UndoRedoStack* stack);

    bool execute(const std::shared_ptr<UICommand>& command, UICommandContext& context);
    bool undo(UICommandContext& context);
    bool redo(UICommandContext& context);

    ListenerId addListener(Listener listener);
    void removeListener(ListenerId id);
};

// TODO [RexUI-Framework-Commands-003]:
// 책임: UI 명령 실행 버스 및 히스토리 연동
// 요구사항:
//  - execute/undo/redo 경로 통합
//  - 이벤트 리스너(명령 완료 알림)
//  - UI는 CommandBus 외 직접 모델 수정 금지
// 의존성:
//  - Commands/UICommand
//  - Commands/UndoRedoStack
// 구현 단계: Phase E
// 성능 고려사항:
//  - 고빈도 명령 시 dispatch 비용 최소화
//  - 리스너 호출 예외 격리
// 테스트 전략:
//  - 실행/취소/재실행 시퀀스 테스트
//  - 리스너 등록/해제 누수 테스트

} // namespace rex::ui::framework::commands
