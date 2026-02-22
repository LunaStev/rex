#pragma once

#include <memory>

namespace rex::ui::framework::binding {
class BindingEngine;
class BindingContext;
}

namespace rex::ui::framework::state {
class UIStateStore;
}

namespace rex::ui::integration::editorbridge {

class EditorViewModel {
public:
    EditorViewModel(
        framework::state::UIStateStore* stateStore,
        framework::binding::BindingEngine* bindingEngine,
        framework::binding::BindingContext* bindingContext);

    void initialize();
    void bindHierarchy();
    void bindInspector();
    void bindToolbar();
};

// TODO [RexUI-Integration-002]:
// 책임: Editor 도메인 상태와 UI 바인딩 구성
// 요구사항:
//  - 계층/인스펙터/툴바 바인딩 구성
//  - 초기 상태 주입 및 동기화
//  - 모듈 경계 밖 직접 모델 접근 금지
// 의존성:
//  - Framework/State/UIStateStore
//  - Framework/Binding/BindingContext
// 구현 단계: Phase C
// 성능 고려사항:
//  - 초기 바인딩 비용 제어
//  - 고빈도 변경 경로 debounce
// 테스트 전략:
//  - 바인딩 초기화 테스트
//  - 상태 변경 반영 테스트

} // namespace rex::ui::integration::editorbridge
