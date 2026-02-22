#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace rex::editor::asset {

struct AssetRecord {
    std::string id;
    std::string path;
    std::string type;
    std::unordered_map<std::string, std::string> metadata;
};

class AssetRegistry {
public:
    bool registerAsset(const AssetRecord& record);
    bool unregisterAsset(const std::string& assetId);

    std::optional<AssetRecord> findById(const std::string& assetId) const;
    std::vector<AssetRecord> queryByPathPrefix(const std::string& prefix) const;
    std::vector<AssetRecord> queryByType(const std::string& type) const;

private:
    std::unordered_map<std::string, AssetRecord> byId_;
};

// TODO [Editor-Asset-001]:
// 책임: 애셋 인덱스/조회 레지스트리 제공
// 요구사항:
//  - id/path/type 기반 조회
//  - 메타데이터 태깅 저장
//  - 빠른 필터 질의 지원
// 의존성:
//  - 없음
// 구현 단계: Phase B
// 성능 고려사항:
//  - 대규모 애셋 조회 O(1)~O(logN)
//  - 인덱스 재구축 비용 분산
// 테스트 전략:
//  - 등록/조회/삭제 테스트
//  - 타입/경로 필터 테스트

} // namespace rex::editor::asset

