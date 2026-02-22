#pragma once

#include <string>

namespace rex::ui::framework::docking {

class DockManager;

class DockSerializer {
public:
    std::string serialize(const DockManager& manager) const;
    bool deserialize(const std::string& data, DockManager& manager) const;
};

// TODO [RexUI-Framework-Docking-003]:
// 책임: 도킹 레이아웃 직렬화/복원
// 요구사항:
//  - 버전 호환 가능한 포맷
//  - 유효성 검증 후 복원
//  - 패널 누락 시 graceful fallback
// 의존성:
//  - Docking/DockManager
// 구현 단계: Phase E
// 성능 고려사항:
//  - 저장/복원 O(N)
//  - 대형 레이아웃 문자열 처리 비용 제어
// 테스트 전략:
//  - 저장/복원 round-trip 테스트
//  - 손상 데이터 복원 실패 테스트

} // namespace rex::ui::framework::docking
