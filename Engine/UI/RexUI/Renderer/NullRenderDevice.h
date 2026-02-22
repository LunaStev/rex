#pragma once

#include <atomic>
#include <cstdint>
#include <unordered_set>

#include "IRenderDevice.h"

namespace rex::ui::renderer {

class NullRenderDevice final : public IRenderDevice {
public:
    std::uint64_t createTexture(const std::string& debugName, std::uint32_t width, std::uint32_t height) override {
        (void)debugName;
        (void)width;
        (void)height;
        const std::uint64_t id = nextId_.fetch_add(1, std::memory_order_relaxed);
        textures_.insert(id);
        return id;
    }

    void destroyTexture(std::uint64_t textureId) override {
        textures_.erase(textureId);
    }

    std::uint64_t createTextAtlas(const std::string& fontName, float fontSize) override {
        (void)fontName;
        (void)fontSize;
        const std::uint64_t id = nextId_.fetch_add(1, std::memory_order_relaxed);
        atlases_.insert(id);
        return id;
    }

    void destroyTextAtlas(std::uint64_t atlasId) override {
        atlases_.erase(atlasId);
    }

private:
    std::atomic<std::uint64_t> nextId_{1};
    std::unordered_set<std::uint64_t> textures_;
    std::unordered_set<std::uint64_t> atlases_;
};

// TODO [RexUI-Renderer-006]:
// 책임: 헤드리스/테스트용 RenderDevice 구현 제공
// 요구사항:
//  - 텍스처/아틀라스 핸들 발급/해제
//  - GPU API 없이 동작
//  - 백엔드 smoke test 경로 제공
// 의존성:
//  - Renderer/IRenderDevice
// 구현 단계: Phase D
// 성능 고려사항:
//  - 핸들 발급 lock-free(atomic) 유지
//  - 테스트 환경 메모리 누수 방지
// 테스트 전략:
//  - 생성/해제 핸들 유효성 테스트
//  - 다중 생성 스트레스 테스트

} // namespace rex::ui::renderer

