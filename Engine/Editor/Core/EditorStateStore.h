#pragma once

#include "../../UI/RexUI/Framework/State/UIStateStore.h"
#include "EditorState.h"

namespace rex::editor::core {

class EditorStateStore {
public:
    using SubscriptionId = ui::framework::state::UIStateStore::SubscriptionId;
    using Callback = ui::framework::state::UIStateStore::Callback;

    EditorState& state();
    const EditorState& state() const;

    ui::framework::state::UIStateStore& rawStore();
    const ui::framework::state::UIStateStore& rawStore() const;

    void beginBatch();
    void endBatch();

    SubscriptionId subscribe(const std::string& pathPrefix, Callback callback);
    void unsubscribe(SubscriptionId id);

private:
    EditorState state_{};
    ui::framework::state::UIStateStore store_{};
};

// TODO [Editor-Core-002]:
// 책임: EditorState + UIStateStore 브리지 제공
// 요구사항:
//  - strongly typed EditorState와 path 기반 스토어 동기화
//  - 배치 업데이트 지원
//  - 패널 바인딩 구독 API 제공
// 의존성:
//  - Editor/Core/EditorState
//  - UI/RexUI/Framework/State/UIStateStore
// 구현 단계: Phase A
// 성능 고려사항:
//  - notify coalescing
//  - 잦은 path 갱신 비용 최소화
// 테스트 전략:
//  - 구독/해지/배치 갱신 테스트
//  - typed-state와 raw-store 동기화 테스트

} // namespace rex::editor::core

