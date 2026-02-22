#include "Timeline.h"

#include <algorithm>

namespace rex::ui::framework::animation {

void Timeline::setDuration(float durationSec) {
    duration_ = std::max(0.0f, durationSec);
}

float Timeline::duration() const {
    return duration_;
}

void Timeline::addTrack(const TimelineTrack& track) {
    tracks_.push_back(track);
}

const std::vector<TimelineTrack>& Timeline::tracks() const {
    return tracks_;
}

TimelinePlayer::TimelinePlayer(const Timeline* timeline)
    : timeline_(timeline) {}

void TimelinePlayer::play() {
    playing_ = true;
}

void TimelinePlayer::stop() {
    playing_ = false;
    currentTime_ = 0.0f;
}

void TimelinePlayer::tick(float dt) {
    if (!playing_ || !timeline_) return;
    currentTime_ += std::max(0.0f, dt);
    const float duration = timeline_->duration();
    if (duration > 0.0f && currentTime_ >= duration) {
        currentTime_ = duration;
        playing_ = false;
    }
}

bool TimelinePlayer::isPlaying() const {
    return playing_;
}

float TimelinePlayer::currentTime() const {
    return currentTime_;
}

} // namespace rex::ui::framework::animation

