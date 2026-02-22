#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "UICommand.h"

namespace rex::ui::framework::commands {

class UndoRedoStack {
public:
    void setCapacity(std::size_t capacity);
    std::size_t capacity() const;

    void push(std::shared_ptr<UICommand> command);
    bool canUndo() const;
    bool canRedo() const;

    std::shared_ptr<UICommand> popUndo();
    std::shared_ptr<UICommand> popRedo();
    void clear();

private:
    std::size_t capacity_ = 256;
    std::vector<std::shared_ptr<UICommand>> undo_;
    std::vector<std::shared_ptr<UICommand>> redo_;
};

// TODO [RexUI-Framework-Commands-002]:
// 책임: Undo/Redo 히스토리 스택 모델 정의
// 요구사항:
//  - 고정 용량(capacity) 정책
//  - push 시 redo branch 폐기 규칙
//  - canUndo/canRedo 실시간 반영
// 의존성:
//  - Commands/UICommand
// 구현 단계: Phase E
// 성능 고려사항:
//  - 스택 조작 O(1)
//  - 큰 커맨드 payload 메모리 관리
// 테스트 전략:
//  - 브랜치 폐기 시나리오 테스트
//  - capacity 초과 시 제거 정책 테스트

} // namespace rex::ui::framework::commands
