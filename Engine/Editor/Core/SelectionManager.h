#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace rex::editor::core {

class SelectionManager {
public:
    void clear();

    void selectEntity(std::uint64_t entityId, bool additive);
    void deselectEntity(std::uint64_t entityId);
    bool isEntitySelected(std::uint64_t entityId) const;
    std::vector<std::uint64_t> selectedEntities() const;

    void selectAsset(const std::string& assetId, bool additive);
    void deselectAsset(const std::string& assetId);
    bool isAssetSelected(const std::string& assetId) const;
    std::vector<std::string> selectedAssets() const;

private:
    std::unordered_set<std::uint64_t> selectedEntities_;
    std::unordered_set<std::string> selectedAssets_;
};

// TODO [Editor-Core-004]:
// 책임: 엔티티/애셋 선택 상태 관리
// 요구사항:
//  - 단일/다중 선택 지원
//  - 선택 해제/조회 API
//  - selection change 이벤트 연동 포인트
// 의존성:
//  - 없음
// 구현 단계: Phase A
// 성능 고려사항:
//  - 대량 다중선택 연산 비용 제어
//  - 해시 충돌 최소화
// 테스트 전략:
//  - additive 선택 시나리오 테스트
//  - 대량 선택/해제 회귀 테스트

} // namespace rex::editor::core

