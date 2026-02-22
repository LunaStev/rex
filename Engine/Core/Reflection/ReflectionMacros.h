#pragma once

#include "TypeRegistry.h"

#define REX_REFLECT_TYPE(TYPE, TYPE_NAME_LITERAL)                                    \
    static void RexRegisterType_##TYPE() {                                           \
        ::rex::core::reflection::TypeMetadata metadata{};                            \
        metadata.name = TYPE_NAME_LITERAL;                                           \
        metadata.size = sizeof(TYPE);                                                \
        ::rex::core::reflection::TypeRegistry::instance().registerType<TYPE>(metadata); \
    }

#define REX_REFLECT_PROPERTY(TYPE, PROP_NAME, PROP_TYPE)                             \
    do {                                                                              \
        (void)sizeof(TYPE::PROP_NAME);                                                \
        (void)sizeof(PROP_TYPE);                                                      \
    } while (0)

// TODO [Core-Reflection-003]:
// 책임: 타입/프로퍼티 등록 매크로 제공
// 요구사항:
//  - 선언부 부담 최소화
//  - 컴파일 타임 검증 훅
//  - 코드 생성 툴 연계 확장 포인트
// 의존성:
//  - Reflection/TypeRegistry
// 구현 단계: Phase D
// 성능 고려사항:
//  - 정적 초기화 비용 관리
//  - 매크로 남용으로 인한 컴파일 시간 제어
// 테스트 전략:
//  - 매크로 등록 동작 테스트
//  - 잘못된 프로퍼티 선언 컴파일 실패 테스트

