#pragma once

namespace rex::gfx {

class ExposureController {
public:
    void setManualExposure(float value) { m_manualExposure = value; }
    void setAutoExposure(bool enabled) { m_autoExposure = enabled; }
    void setSpeed(float upSpeed, float downSpeed);

    void update(float averageLuminance, float dt);
    float exposure() const;

private:
    bool m_autoExposure = false;
    float m_manualExposure = 1.0f;
    float m_currentExposure = 1.0f;
    float m_speedUp = 1.5f;
    float m_speedDown = 1.0f;
};

} // namespace rex::gfx
