#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <string>
#include <variant>

namespace rex::ui::framework::state {

using StatePath = std::string;
using StateValue = std::variant<std::monostate, bool, std::int64_t, double, std::string>;

class UIStateStore {
public:
    using SubscriptionId = std::uint64_t;
    using Callback = std::function<void(const StatePath&, const StateValue&)>;

    bool has(const StatePath& path) const;
    std::optional<StateValue> get(const StatePath& path) const;

    void set(const StatePath& path, const StateValue& value);
    void remove(const StatePath& path);

    SubscriptionId subscribe(const StatePath& pathPrefix, Callback callback);
    void unsubscribe(SubscriptionId id);

    void beginBatch();
    void endBatch();

private:
    struct Subscription {
        StatePath pathPrefix;
        Callback callback;
    };

    void notifyPath(const StatePath& path, const StateValue& value);
    static bool pathMatchesPrefix(const StatePath& path, const StatePath& prefix);

    std::unordered_map<StatePath, StateValue> values_;
    std::unordered_map<SubscriptionId, Subscription> subscriptions_;
    SubscriptionId nextSubscriptionId_ = 1;

    bool batching_ = false;
    std::unordered_set<StatePath> batchedDirtyPaths_;
};

// TODO [RexUI-Framework-State-003]:
// 책임: 전역 UI 상태 저장소(단일 소스) 확정
// 요구사항:
//  - path 기반 set/get/remove
//  - 배치 업데이트 트랜잭션(begin/endBatch)
//  - prefix 구독 + 변경 통지
// 의존성:
//  - 없음
// 구현 단계: Phase C
// 성능 고려사항:
//  - path 인덱싱 효율화
//  - batch flush 시 notify coalescing
// 테스트 전략:
//  - 대량 path 업데이트 테스트
//  - 구독 범위(prefix) 정확성 테스트

} // namespace rex::ui::framework::state
