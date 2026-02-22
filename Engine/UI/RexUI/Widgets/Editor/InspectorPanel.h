#pragma once

#include <functional>
#include <string>
#include <vector>

#include "../../Core/Widget.h"

namespace rex::ui::widgets::editor {

struct InspectorField {
    std::string propertyPath;
    std::string displayName;
    std::string editorType;
};

class InspectorPanel : public core::Widget {
public:
    using CommitHandler = std::function<void(const std::string& propertyPath, const std::string& value)>;

    void setFields(std::vector<InspectorField> fields);
    void setOnCommit(CommitHandler handler);

private:
    std::vector<InspectorField> fields_;
    CommitHandler onCommit_{};
};

// TODO [RexUI-Widgets-Editor-002]:
// 책임: 속성 인스펙터 패널 인터페이스 선언
// 요구사항:
//  - typed field(editorType) 선언
//  - 값 커밋 이벤트를 CommandBus로 전달
//  - validation/error 상태 표시 훅
// 의존성:
//  - Core/Widget
//  - Framework/Commands/CommandBus
// 구현 단계: Phase E
// 성능 고려사항:
//  - 필드 재생성 최소화
//  - 바인딩 업데이트 배치 처리
// 테스트 전략:
//  - 값 커밋/검증 실패 테스트
//  - 다중 필드 갱신 테스트

} // namespace rex::ui::widgets::editor
