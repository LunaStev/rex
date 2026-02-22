#pragma once

namespace rex::editor::core {
class EditorApp;
}

namespace rex::editor::tools {

class IEditorTool {
public:
    virtual ~IEditorTool() = default;
    virtual const char* id() const = 0;
    virtual bool onEnter(core::EditorApp& app) = 0;
    virtual void onExit(core::EditorApp& app) = 0;
    virtual void onTick(core::EditorApp& app, float dt) = 0;
};

// TODO [Editor-Tools-000]:
// 책임: 에디터 툴 공통 인터페이스 제공
// 요구사항:
//  - enter/exit/tick 수명주기
//  - 모드/컨텍스트 전환 대응
//  - 플러그인 등록 가능한 형태
// 의존성:
//  - Editor/Core/EditorApp
// 구현 단계: Phase B
// 성능 고려사항:
//  - 비활성 툴 업데이트 억제
//  - 툴 전환 오버헤드 최소화
// 테스트 전략:
//  - 툴 수명주기 테스트
//  - 툴 전환 회귀 테스트

} // namespace rex::editor::tools

