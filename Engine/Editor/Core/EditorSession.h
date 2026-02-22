#pragma once

#include <cstdint>

#include "EditorWorkspace.h"

namespace rex::editor::core {

class EditorSession {
public:
    bool open(const EditorWorkspace& workspace);
    void close();
    bool isOpen() const;

    std::uint64_t sessionId() const;
    const EditorWorkspace& workspace() const;

private:
    bool open_ = false;
    std::uint64_t sessionId_ = 0;
    EditorWorkspace workspace_{};
};

// TODO [Editor-Core-008]:
// 책임: 에디터 세션 수명주기 관리
// 요구사항:
//  - open/close 상태 전이
//  - 세션 식별자 발급
//  - 워크스페이스 바인딩
// 의존성:
//  - Editor/Core/EditorWorkspace
// 구현 단계: Phase A
// 성능 고려사항:
//  - 세션 전환 시 리소스 해제 누락 방지
//  - 재오픈 경로 비용 제어
// 테스트 전략:
//  - open/close 순서 테스트
//  - 중복 open 방어 테스트

} // namespace rex::editor::core

