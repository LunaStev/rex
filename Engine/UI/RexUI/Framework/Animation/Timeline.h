#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace rex::ui::framework::animation {

struct Keyframe {
    float time = 0.0f;
    float value = 0.0f;
};

struct TimelineTrack {
    std::string propertyPath;
    std::vector<Keyframe> keys;
};

class Timeline {
public:
    void setDuration(float durationSec);
    float duration() const;

    void addTrack(const TimelineTrack& track);
    const std::vector<TimelineTrack>& tracks() const;
};

class TimelinePlayer {
public:
    explicit TimelinePlayer(const Timeline* timeline);

    void play();
    void stop();
    void tick(float dt);

    bool isPlaying() const;
    float currentTime() const;
};

// TODO [RexUI-Framework-Animation-002]:
// 책임: 타임라인 기반 UI 애니메이션 실행 모델
// 요구사항:
//  - 다중 트랙/키프레임 재생
//  - play/stop/seek API
//  - 프레임 tick 시 속성 샘플링
// 의존성:
//  - Animation/Easing
// 구현 단계: Phase E
// 성능 고려사항:
//  - 트랙 샘플링 이진 탐색
//  - 동시 재생 타임라인 수 확장성
// 테스트 전략:
//  - 키프레임 보간 정확성 테스트
//  - 재생/정지/재시작 시나리오 테스트

} // namespace rex::ui::framework::animation
