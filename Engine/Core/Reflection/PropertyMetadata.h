#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace rex::core::reflection {

struct PropertyMetadata {
    std::string name;
    std::string typeName;
    std::size_t offset = 0;
    std::size_t size = 0;
};

struct TypeMetadata {
    std::string name;
    std::size_t size = 0;
    std::vector<PropertyMetadata> properties;
};

// TODO [Core-Reflection-001]:
// 책임: 타입/프로퍼티 메타데이터 구조 정의
// 요구사항:
//  - 타입명/크기/프로퍼티 목록 제공
//  - 프로퍼티 오프셋/타입 메타 보관
//  - 에디터 인스펙터용 필드 정보 제공
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 메타 조회 O(1) 경로 확보
//  - 시작 시 메타 등록 비용 제어
// 테스트 전략:
//  - 메타 조회 정확성 테스트
//  - 프로퍼티 오프셋 유효성 테스트

} // namespace rex::core::reflection
