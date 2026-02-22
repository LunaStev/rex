#pragma once

#include "../Window.h"

namespace rex::core::platform {

using WindowConfig = ::rex::WindowConfig;
using Window = ::rex::Window;

// TODO [Core-Platform-001]:
// 책임: 플랫폼 윈도우 추상화 진입점 제공
// 요구사항:
//  - 기존 Window API 호환
//  - 플랫폼별 구현 분리 확장 포인트
//  - 입력/디스플레이 분리 설계
// 의존성:
//  - Core/Window.h
// 구현 단계: Phase D
// 성능 고려사항:
//  - 폴링 루프 오버헤드 최소화
//  - 리사이즈/이벤트 처리 경합 방지
// 테스트 전략:
//  - 윈도우 생성/파괴 테스트
//  - 이벤트 루프 테스트

} // namespace rex::core::platform

