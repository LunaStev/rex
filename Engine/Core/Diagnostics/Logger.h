#pragma once

#include "../Logger.h"

namespace rex::core::diagnostics {

using LogLevel = ::rex::LogLevel;
using Logger = ::rex::Logger;

// TODO [Core-Diagnostics-001]:
// 책임: Core 진단 로그 API 네임스페이스 표준화
// 요구사항:
//  - 기존 Logger와 호환
//  - 향후 sink/formatter 확장 지점 제공
//  - 상위 모듈 공용 진입점
// 의존성:
//  - Core/Logger.h
// 구현 단계: Phase D
// 성능 고려사항:
//  - 로그 포맷팅 비용 제어
//  - 릴리즈 빌드 로그 레벨 필터
// 테스트 전략:
//  - 로그 레벨 출력 테스트
//  - 포맷 문자열 테스트

} // namespace rex::core::diagnostics

