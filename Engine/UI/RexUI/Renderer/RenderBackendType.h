#pragma once

namespace rex::ui::renderer {

enum class RenderBackendType {
    OpenGL,
    Vulkan,
    RexGraphics
};

// TODO [RexUI-Renderer-004]:
// 책임: 렌더 백엔드 선택 열거형 표준화
// 요구사항:
//  - OpenGL/Vulkan/RexGraphics 식별
//  - 런타임 선택 경로와 1:1 매핑
//  - 설정 파일 직렬화 가능한 값 유지
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 분기 비용 무시 수준 유지
//  - 문자열 변환 최소화
// 테스트 전략:
//  - 백엔드 선택 매핑 테스트
//  - 잘못된 enum 입력 방어 테스트

} // namespace rex::ui::renderer

