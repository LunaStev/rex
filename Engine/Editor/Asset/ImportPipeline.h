#pragma once

#include <string>
#include <vector>

namespace rex::editor::asset {

struct ImportRequest {
    std::string sourcePath;
    std::string destinationPath;
    std::string importer;
};

struct ImportResult {
    bool success = false;
    std::string assetId;
    std::string message;
};

class ImportPipeline {
public:
    ImportResult importAsset(const ImportRequest& request);
    ImportResult reimportAsset(const std::string& assetId);
};

// TODO [Editor-Asset-003]:
// 책임: 애셋 import/reimport 워크플로우 제공
// 요구사항:
//  - importer 타입 기반 처리
//  - reimport tracking
//  - 실패 메시지/복구 경로 제공
// 의존성:
//  - Editor/Asset/AssetRegistry
// 구현 단계: Phase B
// 성능 고려사항:
//  - 비동기 import 큐 연계
//  - 대형 파일 처리 시 메모리 피크 제어
// 테스트 전략:
//  - import/reimport 성공/실패 테스트
//  - 잘못된 포맷 처리 테스트

} // namespace rex::editor::asset

