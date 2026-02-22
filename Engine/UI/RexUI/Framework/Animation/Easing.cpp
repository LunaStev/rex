#include "Easing.h"

#include <algorithm>

namespace rex::ui::framework::animation {

float Easing::evaluate(EasingType type, float t) {
    const float x = std::clamp(t, 0.0f, 1.0f);
    switch (type) {
        case EasingType::Linear:
            return x;
        case EasingType::EaseIn:
            return x * x;
        case EasingType::EaseOut:
            return 1.0f - (1.0f - x) * (1.0f - x);
        case EasingType::EaseInOut:
            return (x < 0.5f) ? (2.0f * x * x) : (1.0f - 2.0f * (1.0f - x) * (1.0f - x));
        case EasingType::CubicBezier:
            return x;
    }
    return x;
}

} // namespace rex::ui::framework::animation

