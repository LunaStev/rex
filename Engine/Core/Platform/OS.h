#pragma once

#include <cstdint>
#include <functional>
#include <thread>

namespace rex::core::platform {

class OS {
public:
    static std::uint64_t currentThreadId() {
        return static_cast<std::uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }

    static std::size_t hardwareConcurrency() {
        return std::thread::hardware_concurrency();
    }
};

// TODO [Core-Platform-003]:
// 책임: OS 공통 쿼리 API 제공
// 요구사항:
//  - thread/process 관련 기본 정보
//  - 플랫폼 분기 캡슐화
//  - 확장 가능한 정적 유틸 구조
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 시스템 호출 빈도 관리
//  - 캐시 가능한 값 재사용
// 테스트 전략:
//  - 스레드 ID 조회 테스트
//  - 코어 수 조회 테스트

} // namespace rex::core::platform
