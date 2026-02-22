#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace rex::core::module {

struct ModuleVersion {
    std::uint16_t major = 0;
    std::uint16_t minor = 0;
    std::uint16_t patch = 0;
};

struct ModuleDescriptor {
    std::string name;
    ModuleVersion version{};
    std::vector<std::string> dependencies;
};

class IModule {
public:
    virtual ~IModule() = default;
    virtual const ModuleDescriptor& descriptor() const = 0;
    virtual bool startup() = 0;
    virtual void shutdown() = 0;
};

// TODO [Core-Module-001]:
// 책임: 모듈 ABI/버전 메타데이터 규약 정의
// 요구사항:
//  - 모듈 식별자/버전/의존성 구조
//  - startup/shutdown 수명주기 계약
//  - ABI 호환성 검사 확장 포인트
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 모듈 조회 O(1)
//  - 의존 그래프 로딩 비용 관리
// 테스트 전략:
//  - 버전 호환 검사 테스트
//  - 의존성 누락 실패 테스트

} // namespace rex::core::module

