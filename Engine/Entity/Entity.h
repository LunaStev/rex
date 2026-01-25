#pragma once
#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../World/World.h"

class Entity {
protected:
    float x, y;          // Location
    float vx, vy;        // Speed
    float width, height; // Size
    bool alive;          // Active status
    bool onGround;        // Is on ground
    Uint8 r, g, b, a;    // Color (Temporary)

public:
    Entity(float x=0, float y=0, float w=40, float h=40)
        : x(x), y(y), vx(0), vy(0), width(w), height(h),
          alive(true), onGround(false), 
          r(255), g(255), b(255), a(255) {}

    virtual ~Entity() = default;

    virtual void update(float dt, Input& input, World& world);
    virtual void render(Graphics& g);

    // --- Location related ---
    inline float getX() const { return x; }
    inline float getY() const { return y; }
    inline void setPosition(float nx, float ny) { x = nx; y = ny; }

    // --- Related to speed ---
    inline float getVX() const { return vx; }
    inline float getVY() const { return vy; }
    inline void setVX(float nvx) { vx = nvx; }
    inline void setVY(float nvy) { vy = nvy; }
    inline void setVelocity(float nvx, float nvy) { vx = nvx; vy = nvy; }

    // --- Size ---
    inline float getWidth() const { return width; }
    inline float getHeight() const { return height; }

    // --- Color ---
    inline void setColor(Uint8 nr, Uint8 ng, Uint8 nb, Uint8 na=255) { r=nr; g=ng; b=nb; a=na; }

    // --- A state of life ---
    inline bool isAlive() const { return alive; }
    inline void kill() { alive = false; }

    inline bool isOnGround() const { return onGround; }
    inline void setOnGround(bool v) { onGround = v; }
};
