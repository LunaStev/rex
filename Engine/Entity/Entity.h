#pragma once
#include <SDL2/SDL.h>
#include <cmath>

#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../World/World.h"

#include "EntityId.h"

class EntityManager;

class Entity {
    friend class EntityManager;

protected:
    // ---- Transform / Physics (AABB top-left) ----
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float width  = 40.0f;
    float height = 40.0f;

    // ---- State ----
    bool alive = true;
    bool onGround = false;
    bool visible = true;
    bool collidable = true;

    // ---- Debug render fallback (✅ 이름 충돌 방지) ----
    Uint8 colorR = 255, colorG = 255, colorB = 255, colorA = 255;

    // ---- Render (Sprite optional) ----
    bool useSprite = false;
    Sprite sprite{};
    float rotation = 0.0f;     // degrees
    float scale = 1.0f;        // local scale
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    // ---- Identity ----
    EntityId id = NullEntity;

protected:
    void _setId(EntityId v) { id = v; }

public:
    Entity(float x=0, float y=0, float w=40, float h=40)
        : x(x), y(y), width(w), height(h) {}

    virtual ~Entity() = default;

    // ---- Lifecycle hooks ----
    virtual void onSpawn() {}
    virtual void onDestroy() {}

    // ---- Simulation ----
    virtual void update(float dt, Input& input, World& world);

    // ---- Render ----
    virtual void render(Graphics& graphics);

    // ---- Identity ----
    EntityId getId() const { return id; }

    // ---- Location ----
    float getX() const { return x; }
    float getY() const { return y; }
    void  setPosition(float nx, float ny) { x = nx; y = ny; }

    float getCenterX() const { return x + width * 0.5f; }
    float getCenterY() const { return y + height * 0.5f; }

    // ---- Velocity ----
    float getVX() const { return vx; }
    float getVY() const { return vy; }
    void  setVX(float nvx) { vx = nvx; }
    void  setVY(float nvy) { vy = nvy; }
    void  setVelocity(float nvx, float nvy) { vx = nvx; vy = nvy; }

    // ---- Size / Bounds ----
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    void  setSize(float w, float h) { width = w; height = h; }

    SDL_FRect getAABB() const { return SDL_FRect{ x, y, width, height }; }

    // ---- Render state ----
    void setVisible(bool v) { visible = v; }
    bool isVisible() const { return visible; }

    void setCollidable(bool v) { collidable = v; }
    bool isCollidable() const { return collidable; }

    void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a=255) {
        colorR=r; colorG=g; colorB=b; colorA=a;
    }

    void setRotation(float deg) { rotation = deg; }
    float getRotation() const { return rotation; }

    void setScale(float s) { scale = (s < 0.0f ? 0.0f : s); }
    float getScale() const { return scale; }

    void setFlip(SDL_RendererFlip f) { flip = f; }
    SDL_RendererFlip getFlip() const { return flip; }

    void clearSprite() { useSprite = false; sprite = Sprite(); }
    void setSprite(const Sprite& sp) { sprite = sp; useSprite = sp.valid(); }
    bool hasSprite() const { return useSprite && sprite.valid(); }

    // ---- Alive ----
    bool isAlive() const { return alive; }
    void kill() { alive = false; }

    bool isOnGround() const { return onGround; }
    void setOnGround(bool v) { onGround = v; }
};
