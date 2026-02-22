#pragma once

#include <string>

#include "../../UI/RexUI/Framework/Docking/DockManager.h"
#include "../../UI/RexUI/Framework/Docking/DockSerializer.h"

namespace rex::editor::core {

class LayoutService {
public:
    std::string save(const ui::framework::docking::DockManager& dockManager) const;
    bool restore(const std::string& blob, ui::framework::docking::DockManager& dockManager) const;

private:
    ui::framework::docking::DockSerializer serializer_{};
};

// TODO [Editor-Core-006]:
// 책임: 도킹 레이아웃 저장/복원 서비스 제공
// 요구사항:
//  - 직렬화/복원 API
//  - 손상 데이터 graceful fallback
//  - 프로젝트별 레이아웃 버전 관리 포인트
// 의존성:
//  - UI/RexUI/Framework/Docking/*
// 구현 단계: Phase A
// 성능 고려사항:
//  - 대형 레이아웃 저장/복원 O(N)
//  - 문자열 복사 최소화
// 테스트 전략:
//  - round-trip 테스트
//  - 손상 blob 복원 실패 테스트

} // namespace rex::editor::core

