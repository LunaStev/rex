#pragma once

#include <cstdint>

namespace rex::ui::core {

struct Point {
    float x = 0.0f;
    float y = 0.0f;
};

struct Size {
    float w = 0.0f;
    float h = 0.0f;
};

struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    bool contains(const Point& p) const {
        return p.x >= x && p.x <= x + w && p.y >= y && p.y <= y + h;
    }
};

struct Margin {
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
};

struct LayoutConstraints {
    Size min{};
    Size max{};
};

struct LayoutResult {
    Size desired{};
    Rect arranged{};
    bool cacheHit = false;
};

// TODO [RexUI-Core-002]:
// 책임: UI 레이아웃/히트테스트 기하 타입 표준화
// 요구사항:
//  - 픽셀/DP 단위 변환 경계 정의
//  - 레이아웃 제약(min/max) 공통 규약
//  - 히트테스트와 클리핑용 기하 유틸
// 의존성:
//  - 없음
// 구현 단계: Phase A
// 성능 고려사항:
//  - hot path에서 분기 최소화
//  - SIMD 확장 가능성 확보
// 테스트 전략:
//  - 경계값(음수/0/대형 좌표) 테스트
//  - contains/교차 판정 회귀 테스트

} // namespace rex::ui::core
