#pragma once

#include <chrono>
#include <functional>
#include <string>

namespace rex::core::diagnostics {

class ProfilerHooks {
public:
    using ScopeCallback = std::function<void(const std::string&, double)>;

    static void setScopeCallback(ScopeCallback cb) {
        callback() = std::move(cb);
    }

    static void emitScope(const std::string& name, double ms) {
        if (callback()) {
            callback()(name, ms);
        }
    }

private:
    static ScopeCallback& callback() {
        static ScopeCallback cb{};
        return cb;
    }
};

class ScopedProfile {
public:
    explicit ScopedProfile(std::string name)
        : name_(std::move(name))
        , begin_(Clock::now()) {}

    ~ScopedProfile() {
        const auto end = Clock::now();
        const auto elapsed = std::chrono::duration<double, std::milli>(end - begin_);
        ProfilerHooks::emitScope(name_, elapsed.count());
    }

private:
    using Clock = std::chrono::steady_clock;
    std::string name_;
    Clock::time_point begin_{};
};

// TODO [Core-Diagnostics-004]:
// 책임: 프로파일링 훅 및 scoped 타이머 제공
// 요구사항:
//  - 스코프 단위 시간 측정
//  - 외부 프로파일러 sink 연동
//  - 문자열 기반 샘플 식별
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 타이머 호출 비용 최소화
//  - 릴리즈 빌드 비활성화 경로 제공
// 테스트 전략:
//  - 측정값 범위 테스트
//  - 콜백 호출 테스트

} // namespace rex::core::diagnostics

