#pragma once

#include <functional>
#include <string>

namespace rex::core::diagnostics {

class CrashHandler {
public:
    using Callback = std::function<void(const std::string&)>;

    static void setHandler(Callback cb) {
        handler() = std::move(cb);
    }

    static void reportFatal(const std::string& message) {
        if (handler()) {
            handler()(message);
        }
    }

private:
    static Callback& handler() {
        static Callback cb{};
        return cb;
    }
};

// TODO [Core-Diagnostics-003]:
// 책임: 치명 오류 리포트 핸들러 제공
// 요구사항:
//  - 콜백 등록/호출 API
//  - 플랫폼별 크래시 리포터 연결 포인트
//  - 최소한의 메시지 전달 보장
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 정상 경로 오버헤드 0에 가깝게 유지
//  - 실패 경로에서 재진입 방지
// 테스트 전략:
//  - 핸들러 등록/호출 테스트
//  - null 핸들러 경계 테스트

} // namespace rex::core::diagnostics

