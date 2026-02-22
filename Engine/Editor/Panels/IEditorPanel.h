#pragma once

#include <string>

namespace rex::editor::core {
class EditorApp;
}

namespace rex::editor::panels {

class IEditorPanel {
public:
    virtual ~IEditorPanel() = default;
    virtual const char* id() const = 0;
    virtual const char* title() const = 0;

    virtual bool onAttach(core::EditorApp& app) = 0;
    virtual void onDetach(core::EditorApp& app) = 0;
    virtual void onTick(core::EditorApp& app, float dt) = 0;
};

// TODO [Editor-Panels-000]:
// 책임: 모든 에디터 패널의 공통 수명주기 인터페이스 제공
// 요구사항:
//  - attach/detach/tick 계약
//  - 패널 식별자/id/title 제공
//  - 플러그인 등록 가능한 ABI 안정성 고려
// 의존성:
//  - Editor/Core/EditorApp
// 구현 단계: Phase A
// 성능 고려사항:
//  - 패널 tick 디스패치 비용 최소화
//  - 비활성 패널 업데이트 억제
// 테스트 전략:
//  - 패널 attach/detach 테스트
//  - id 중복 등록 방어 테스트

} // namespace rex::editor::panels

