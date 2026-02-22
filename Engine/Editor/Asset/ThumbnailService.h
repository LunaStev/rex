#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace rex::editor::asset {

struct ThumbnailHandle {
    std::uint64_t id = 0;
};

class ThumbnailService {
public:
    void request(const std::string& assetId);
    std::optional<ThumbnailHandle> get(const std::string& assetId) const;
};

// TODO [Editor-Asset-004]:
// 책임: 애셋 썸네일 비동기 생성/조회 서비스
// 요구사항:
//  - 요청 큐 + 결과 조회 API
//  - 백그라운드 생성 확장 포인트
//  - 캐시/만료 정책
// 의존성:
//  - Editor/Asset/AssetRegistry
//  - Graphics
// 구현 단계: Phase B
// 성능 고려사항:
//  - UI thread 비차단
//  - 대량 요청 중 중복 생성 억제
// 테스트 전략:
//  - 요청/완료 흐름 테스트
//  - 캐시 히트/미스 테스트

} // namespace rex::editor::asset

