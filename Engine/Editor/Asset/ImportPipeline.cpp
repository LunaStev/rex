#include "ImportPipeline.h"

namespace rex::editor::asset {

ImportResult ImportPipeline::importAsset(const ImportRequest& request) {
    ImportResult result{};
    if (request.sourcePath.empty() || request.destinationPath.empty()) {
        result.success = false;
        result.message = "invalid import request";
        return result;
    }

    result.success = true;
    result.assetId = request.destinationPath;
    result.message = "imported";
    return result;
}

ImportResult ImportPipeline::reimportAsset(const std::string& assetId) {
    ImportResult result{};
    if (assetId.empty()) {
        result.success = false;
        result.message = "invalid asset id";
        return result;
    }
    result.success = true;
    result.assetId = assetId;
    result.message = "reimported";
    return result;
}

} // namespace rex::editor::asset

