#pragma once

#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../Core/Widget.h"

namespace rex::ui::framework::declarative {

struct ViewNodeDesc {
    std::string type;
    std::string key;
    std::unordered_map<std::string, std::any> props;
    std::vector<ViewNodeDesc> children;
};

class UIBuilder {
public:
    using WidgetPtr = std::shared_ptr<core::Widget>;

    WidgetPtr build(const ViewNodeDesc& rootDesc) const;
    WidgetPtr patch(const WidgetPtr& currentRoot, const ViewNodeDesc& nextDesc) const;
};

// TODO [RexUI-Framework-Declarative-001]:
// 책임: 선언형 뷰 기술(ViewNodeDesc) -> Widget 트리 변환
// 요구사항:
//  - type/key 기반 위젯 인스턴스화
//  - props 바인딩 연결 포인트 제공
//  - patch 입력 시 최소 갱신 경로 제공
// 의존성:
//  - Core/Widget
// 구현 단계: Phase C
// 성능 고려사항:
//  - 대형 트리 build/patch 할당 최소화
//  - key lookup O(1) 근접
// 테스트 전략:
//  - 선언형 입력 대비 트리 생성 스냅샷
//  - patch 후 동일성/참조 보존 테스트

} // namespace rex::ui::framework::declarative
