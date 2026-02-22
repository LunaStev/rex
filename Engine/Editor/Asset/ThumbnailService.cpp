#include "ThumbnailService.h"

#include <unordered_map>

namespace rex::editor::asset {

namespace {
std::unordered_map<std::string, ThumbnailHandle>& mapRef() {
    static std::unordered_map<std::string, ThumbnailHandle> m;
    return m;
}

std::uint64_t& nextIdRef() {
    static std::uint64_t id = 1;
    return id;
}
}

void ThumbnailService::request(const std::string& assetId) {
    if (assetId.empty()) return;
    auto& m = mapRef();
    if (m.find(assetId) != m.end()) return;
    m[assetId] = {nextIdRef()++};
}

std::optional<ThumbnailHandle> ThumbnailService::get(const std::string& assetId) const {
    auto& m = mapRef();
    const auto it = m.find(assetId);
    if (it == m.end()) return std::nullopt;
    return it->second;
}

} // namespace rex::editor::asset

