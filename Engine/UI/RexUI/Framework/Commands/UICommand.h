#pragma once

#include <string>

namespace rex::ui::framework::commands {

class UICommandContext;

class UICommand {
public:
    virtual ~UICommand() = default;

    virtual const char* name() const = 0;
    virtual bool execute(UICommandContext& ctx) = 0;
    virtual bool undo(UICommandContext& ctx) = 0;
    virtual bool redo(UICommandContext& ctx) = 0;
};

class UICommandContext {
public:
    void* userData = nullptr;
};

// TODO [RexUI-Framework-Commands-001]:
// 책임: Command 패턴 기본 인터페이스 고정
// 요구사항:
//  - execute/undo/redo 계약
//  - 명령 이름/메타 정보 제공
//  - 실패 시 복구 정책 정의
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 명령 객체 할당 전략(풀링)
//  - 대량 히스토리 메모리 상한
// 테스트 전략:
//  - 명령 성공/실패/롤백 테스트
//  - 직렬 실행 일관성 테스트

} // namespace rex::ui::framework::commands
