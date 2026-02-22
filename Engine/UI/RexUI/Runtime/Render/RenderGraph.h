#pragma once

#include "DrawCommand.h"

namespace rex::ui::runtime::render {

class RenderGraph {
public:
    void beginFrame();
    void addDrawList(const DrawList& drawList);
    const DrawList& flattened() const;
    void clear();
};

// TODO [RexUI-Runtime-Render-003]:
// 책임: 다중 패스/다중 레이어 DrawList 합성
// 요구사항:
//  - 프레임 begin/end 수명주기
//  - 레이어/오버레이 병합
//  - 백엔드 제출용 평탄화
// 의존성:
//  - Runtime/Render/DrawCommand
// 구현 단계: Phase D
// 성능 고려사항:
//  - 병합 중 복사 최소화
//  - 대규모 명령 수에서 선형 처리 유지
// 테스트 전략:
//  - 레이어 병합 순서 테스트
//  - 프레임 초기화 누락 테스트

} // namespace rex::ui::runtime::render
