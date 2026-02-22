#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace rex::editor::asset {

class DependencyGraph {
public:
    void addDependency(const std::string& fromAssetId, const std::string& toAssetId);
    void removeAsset(const std::string& assetId);

    std::vector<std::string> dependenciesOf(const std::string& assetId) const;
    std::vector<std::string> dependentsOf(const std::string& assetId) const;

private:
    std::unordered_map<std::string, std::unordered_set<std::string>> outgoing_;
    std::unordered_map<std::string, std::unordered_set<std::string>> incoming_;
};

// TODO [Editor-Asset-002]:
// 책임: 애셋 의존성 그래프 관리
// 요구사항:
//  - 참조 방향 그래프 유지
//  - dependency/dependent 조회
//  - 삭제 시 엣지 정리
// 의존성:
//  - 없음
// 구현 단계: Phase B
// 성능 고려사항:
//  - 대형 그래프 조회 최적화
//  - 순환 의존 탐지 확장 포인트
// 테스트 전략:
//  - 의존성 추가/삭제 테스트
//  - 영향 범위 조회 테스트

} // namespace rex::editor::asset

