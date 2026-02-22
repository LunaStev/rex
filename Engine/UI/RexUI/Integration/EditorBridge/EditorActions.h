#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace rex::ui::framework::commands {
class UICommand;
}

namespace rex::ui::integration::editorbridge {

class EditorActions {
public:
    std::shared_ptr<framework::commands::UICommand> makeAddEntityAction(const std::string& archetype) const;
    std::shared_ptr<framework::commands::UICommand> makeDeleteSelectedAction() const;
    std::shared_ptr<framework::commands::UICommand> makeSetGizmoModeAction(const std::string& mode) const;
    std::shared_ptr<framework::commands::UICommand> makeApplyTransformAction(
        std::uint64_t entityId,
        const std::string& position,
        const std::string& rotation,
        const std::string& scale) const;
};

// TODO [RexUI-Integration-003]:
// 책임: 에디터 액션을 Command 객체로 표준화
// 요구사항:
//  - UI 이벤트 -> Command 생성 팩토리
//  - Undo/Redo 가능한 payload 구성
//  - 도메인 검증 실패 시 에러 반환 계약
// 의존성:
//  - Framework/Commands/UICommand
// 구현 단계: Phase E
// 성능 고려사항:
//  - Command 생성 시 불필요 복사 최소화
//  - 반복 액션 객체 재사용 전략
// 테스트 전략:
//  - 액션별 command 생성 테스트
//  - undo/redo payload 정확성 테스트

} // namespace rex::ui::integration::editorbridge
