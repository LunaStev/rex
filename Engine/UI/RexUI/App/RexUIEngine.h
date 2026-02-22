#pragma once

#include <cstdint>
#include <memory>

#include "../Core/Widget.h"
#include "../Framework/Binding/BindingContext.h"
#include "../Framework/Binding/BindingExpression.h"
#include "../Framework/State/UIStateStore.h"
#include "../Renderer/IRenderBackend.h"
#include "../Runtime/Layout/ConstraintSolver.h"
#include "../Runtime/Layout/LayoutCache.h"
#include "../Runtime/Layout/LayoutEngine.h"
#include "../Runtime/Render/DrawListBuilder.h"
#include "../Runtime/Render/RenderGraph.h"
#include "../Runtime/Tree/WidgetTree.h"

namespace rex::ui::app {

class RexUIEngine {
public:
    explicit RexUIEngine(renderer::IRenderBackend* backend);

    void setRoot(const std::shared_ptr<core::Widget>& root);
    std::shared_ptr<core::Widget> root() const;

    void setViewport(std::uint32_t width, std::uint32_t height);

    framework::state::UIStateStore& stateStore();
    framework::binding::BindingEngine& bindingEngine();
    framework::binding::BindingContext& bindingContext();

    runtime::tree::WidgetTree& widgetTree();
    const runtime::tree::WidgetTree& widgetTree() const;

    bool runFrame(float dt, std::uint64_t frameIndex);

private:
    renderer::IRenderBackend* backend_ = nullptr;
    std::uint32_t viewportWidth_ = 0;
    std::uint32_t viewportHeight_ = 0;

    runtime::tree::WidgetTree widgetTree_{};
    runtime::layout::ConstraintSolver constraintSolver_{};
    runtime::layout::LayoutCache layoutCache_{};
    runtime::layout::LayoutEngine layoutEngine_{&constraintSolver_, &layoutCache_};
    runtime::render::DrawListBuilder drawBuilder_{};
    runtime::render::RenderGraph renderGraph_{};

    framework::state::UIStateStore stateStore_{};
    framework::binding::BindingContext bindingContext_{&stateStore_, nullptr};
    framework::binding::BindingEngine bindingEngine_{};
};

// TODO [RexUI-App-001]:
// 책임: RexUI 프레임 오케스트레이션 단일 엔트리 제공
// 요구사항:
//  - root/widget tree/lifecycle 관리
//  - layout -> draw list -> backend submit 파이프라인
//  - state/binding 런타임 컨텍스트 제공
// 의존성:
//  - Runtime/Tree, Layout, Render
//  - Framework/State, Binding
//  - Renderer/IRenderBackend
// 구현 단계: Phase D
// 성능 고려사항:
//  - 프레임당 재할당 최소화
//  - 뷰포트 변경 외 불필요 layout 억제
// 테스트 전략:
//  - headless frame 실행 테스트
//  - root 교체/바인딩 안정성 테스트

} // namespace rex::ui::app

