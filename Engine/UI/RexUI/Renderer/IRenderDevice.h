#pragma once

#include <cstdint>
#include <string>

namespace rex::ui::renderer {

class IRenderDevice {
public:
    virtual ~IRenderDevice() = default;

    virtual std::uint64_t createTexture(const std::string& debugName, std::uint32_t width, std::uint32_t height) = 0;
    virtual void destroyTexture(std::uint64_t textureId) = 0;

    virtual std::uint64_t createTextAtlas(const std::string& fontName, float fontSize) = 0;
    virtual void destroyTextAtlas(std::uint64_t atlasId) = 0;
};

// TODO [RexUI-Renderer-001]:
// 책임: 렌더 디바이스 리소스 추상화
// 요구사항:
//  - 텍스처/텍스트 아틀라스 생성/삭제 인터페이스
//  - 백엔드 공통 핸들 타입 규약
//  - 디버그 이름 추적 지원
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 리소스 생성/삭제 배치 처리
//  - 프레임 중 동기화 스톨 최소화
// 테스트 전략:
//  - 리소스 누수 테스트
//  - 다중 생성/삭제 스트레스 테스트

} // namespace rex::ui::renderer
