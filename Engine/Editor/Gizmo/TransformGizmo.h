#pragma once

#include "../../Core/RexMath.h"

namespace rex::editor::gizmo {

enum class GizmoOperation {
    Translate,
    Rotate,
    Scale
};

enum class GizmoSpace {
    Local,
    World
};

struct GizmoSnapSettings {
    bool gridSnap = false;
    bool rotationSnap = false;
    bool scaleSnap = false;
    float gridStep = 1.0f;
    float rotationStepDeg = 15.0f;
    float scaleStep = 0.1f;
};

class TransformGizmo {
public:
    void setOperation(GizmoOperation op);
    GizmoOperation operation() const;

    void setSpace(GizmoSpace space);
    GizmoSpace space() const;

    void setSnap(const GizmoSnapSettings& snap);
    const GizmoSnapSettings& snap() const;

private:
    GizmoOperation operation_ = GizmoOperation::Translate;
    GizmoSpace space_ = GizmoSpace::World;
    GizmoSnapSettings snap_{};
};

// TODO [Editor-Gizmo-001]:
// 책임: Transform Gizmo 상태/스냅 설정 관리
// 요구사항:
//  - translate/rotate/scale 연산 상태
//  - local/world space 전환
//  - grid/rotation/scale snap 설정
// 의존성:
//  - Core/RexMath
// 구현 단계: Phase B
// 성능 고려사항:
//  - 드래그 중 연산 분기 최소화
//  - 다중 선택 변환 비용 제어
// 테스트 전략:
//  - 스냅 동작 정확성 테스트
//  - local/world 변환 테스트

} // namespace rex::editor::gizmo

