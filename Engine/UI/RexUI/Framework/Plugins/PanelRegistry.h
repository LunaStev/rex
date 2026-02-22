#pragma once

#include <string>

namespace rex::ui::framework::plugins {

struct PanelDescriptor {
    std::string id;
    std::string title;
    std::string defaultDockArea;
    bool defaultVisible = true;
};

class PanelRegistry {
public:
    bool registerPanel(const PanelDescriptor& descriptor);
    void unregisterPanel(const std::string& panelId);
    bool hasPanel(const std::string& panelId) const;
    PanelDescriptor getPanel(const std::string& panelId) const;
};

// TODO [RexUI-Framework-Plugins-002]:
// 책임: 에디터 패널 메타데이터 등록소
// 요구사항:
//  - 패널 id/title/dock hint 보관
//  - 기본 가시성/기본 위치 정책
//  - panel id 유일성 보장
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 패널 조회 O(1)
//  - 직렬화용 메타 접근 비용 최소화
// 테스트 전략:
//  - 패널 등록/중복/id 충돌 테스트
//  - 조회 실패 경로 테스트

} // namespace rex::ui::framework::plugins
