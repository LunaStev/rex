#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

namespace rex::editor::core {

enum class EditorMode {
    Select,
    Landscape,
    Modeling,
    Animation
};

enum class ViewportRenderMode {
    Lit,
    Wireframe,
    LightingOnly
};

struct EditorState {
    std::string projectPath;
    std::string activeScenePath;
    bool sceneDirty = false;

    EditorMode mode = EditorMode::Select;
    ViewportRenderMode viewportRenderMode = ViewportRenderMode::Lit;
    bool postProcessEnabled = true;

    std::unordered_set<std::uint64_t> selectedEntities;
    std::unordered_set<std::string> selectedAssets;
};

// TODO [Editor-Core-001]:
// 책임: 에디터 전역 상태 모델 정의
// 요구사항:
//  - 프로젝트/씬/선택/렌더 모드 상태 포함
//  - Dirty 상태 추적
//  - 직렬화 가능한 상태 구조 유지
// 의존성:
//  - 없음
// 구현 단계: Phase A
// 성능 고려사항:
//  - 대량 선택 상태 변경 비용 최소화
//  - 문자열 복사 억제
// 테스트 전략:
//  - 상태 전환 회귀 테스트
//  - Dirty 플래그 일관성 테스트

} // namespace rex::editor::core

