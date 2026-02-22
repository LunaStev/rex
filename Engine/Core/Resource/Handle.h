#pragma once

#include <cstdint>

namespace rex::core::resource {

struct HandleId {
    std::uint32_t index = 0;
    std::uint32_t generation = 0;

    explicit operator bool() const {
        return generation != 0;
    }
};

template <typename TTag>
struct StrongHandle {
    HandleId id{};
    explicit operator bool() const { return static_cast<bool>(id); }
};

template <typename TTag>
struct WeakHandle {
    HandleId id{};
    explicit operator bool() const { return static_cast<bool>(id); }
};

// TODO [Core-Resource-001]:
// 책임: 강/약 리소스 핸들 표현 타입 정의
// 요구사항:
//  - index+generation 기반 식별
//  - Strong/Weak 핸들 분리
//  - 무효 핸들 판별 API
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 핸들 크기 최소화
//  - 복사 비용 무시 수준 유지
// 테스트 전략:
//  - 유효/무효 판별 테스트
//  - 세대 증가 검증 테스트

} // namespace rex::core::resource

