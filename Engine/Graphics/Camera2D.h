#pragma once
#include <algorithm>

class Camera2D {
public:
    float x = 0.0f;
    float y = 0.0f;

    float zoom = 1.0f;

    void setPosition(float nx, float ny) { x = nx; y = ny; }
    void move(float dx, float dy) { x += dx; y += dy; }

    void setZoom(float z) { zoom = std::clamp(z, 0.05f, 20.0f); }
    void addZoom(float dz) { setZoom(zoom + dz); }
};
