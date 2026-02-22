#pragma once

#include "../../Core/Widget.h"

namespace rex::ui::widgets::basic {

enum class PanelOrientation {
    Horizontal,
    Vertical,
    Overlay
};

class PanelWidget : public core::Widget {
public:
    void setOrientation(PanelOrientation orientation);
    PanelOrientation orientation() const;
};

// TODO [RexUI-Widgets-Basic-001]:
// 책임: 기본 컨테이너 패널 위젯 선언
// 요구사항:
//  - 방향(수평/수직/오버레이) 설정
//  - 자식 배치 규칙 연결
//  - 스타일 클래스 기반 배경/테두리 반영
// 의존성:
//  - Core/Widget
// 구현 단계: Phase A
// 성능 고려사항:
//  - 자식 순회 비용 최소화
//  - layout cache 히트율 확보
// 테스트 전략:
//  - 방향별 배치 스냅샷 테스트
//  - 자식 추가/삭제 회귀 테스트

} // namespace rex::ui::widgets::basic
