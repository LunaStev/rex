#pragma once

#include <cstdlib>

#include "Logger.h"

#define REX_ASSERT(EXPR, FMT, ...)                                              \
    do {                                                                        \
        if (!(EXPR)) {                                                          \
            ::rex::core::diagnostics::Logger::error("Assertion failed: " FMT, ##__VA_ARGS__); \
            std::abort();                                                       \
        }                                                                       \
    } while (0)

// TODO [Core-Diagnostics-002]:
// 책임: 공통 assertion 매크로 제공
// 요구사항:
//  - 실패 시 로그 + 중단 동작
//  - 빌드 설정별 활성화 정책 확장
//  - 호출 위치 정보 제공 확장
// 의존성:
//  - Diagnostics/Logger
// 구현 단계: Phase D
// 성능 고려사항:
//  - 릴리즈에서 assert 비용 최소화
//  - 실패 경로에서 정보 손실 방지
// 테스트 전략:
//  - assert 성공/실패 동작 테스트
//  - 포맷 문자열 테스트

