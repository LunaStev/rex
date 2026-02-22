#include "ExposureController.h"

#include <algorithm>
#include <cmath>

namespace rex::gfx {

void ExposureController::setSpeed(float upSpeed, float downSpeed) {
    m_speedUp = std::max(0.01f, upSpeed);
    m_speedDown = std::max(0.01f, downSpeed);
}

void ExposureController::update(float averageLuminance, float dt) {
    if (!m_autoExposure) {
        m_currentExposure = m_manualExposure;
        return;
    }

    const float lum = std::max(0.001f, averageLuminance);
    const float target = 0.6f / lum;

    const float speed = (target > m_currentExposure) ? m_speedUp : m_speedDown;
    const float alpha = 1.0f - std::exp(-speed * std::max(0.0f, dt));
    m_currentExposure = m_currentExposure + (target - m_currentExposure) * alpha;
}

float ExposureController::exposure() const {
    return m_autoExposure ? m_currentExposure : m_manualExposure;
}

} // namespace rex::gfx
