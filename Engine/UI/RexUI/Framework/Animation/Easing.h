#pragma once

namespace rex::ui::framework::animation {

enum class EasingType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    CubicBezier
};

class Easing {
public:
    static float evaluate(EasingType type, float t);
};

// TODO [RexUI-Framework-Animation-001]:
// 책임: 공통 easing 함수 집합 정의
// 요구사항:
//  - 기본 easing 함수(linear/ease) 지원
//  - 커스텀 cubic bezier 확장
//  - t 범위 클램프 규약
// 의존성:
//  - 없음
// 구현 단계: Phase E
// 성능 고려사항:
//  - 프레임당 다수 평가 최적화
//  - LUT(look-up table) 옵션
// 테스트 전략:
//  - easing 곡선 샘플 검증
//  - 경계값 t=0/1 테스트

} // namespace rex::ui::framework::animation
