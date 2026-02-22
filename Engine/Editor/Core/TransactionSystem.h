#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace rex::editor::core {

struct TransactionRecord {
    std::string label;
    std::uint64_t commandCount = 0;
};

class TransactionSystem {
public:
    void begin(const std::string& label);
    void markCommandPushed();
    TransactionRecord end();
    bool inTransaction() const;

    const std::vector<TransactionRecord>& history() const;

private:
    bool open_ = false;
    TransactionRecord current_{};
    std::vector<TransactionRecord> history_{};
};

// TODO [Editor-Core-005]:
// 책임: 에디터 편집 트랜잭션 경계 관리
// 요구사항:
//  - begin/end 트랜잭션 API
//  - 트랜잭션 내 명령 수 추적
//  - 히스토리 뷰 모델 연동
// 의존성:
//  - Editor/Core/EditorCommandBus
// 구현 단계: Phase A
// 성능 고려사항:
//  - 트랜잭션 메타데이터 메모리 상한
//  - 중첩 트랜잭션 정책 고정
// 테스트 전략:
//  - begin/end 짝 테스트
//  - undo/redo 경계 일관성 테스트

} // namespace rex::editor::core

