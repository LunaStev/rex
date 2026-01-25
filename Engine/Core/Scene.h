#pragma once
#include "Game.h"

class Engine; // forward

class Scene : public Game {
public:
    virtual ~Scene() = default;

    // Called when scene becomes active
    virtual void onEnter(Engine& engine) { (void)engine; }

    // Called when scene is removed (replaced/popped)
    virtual void onExit(Engine& engine) { (void)engine; }

    // Called when another scene is pushed on top
    virtual void onPause(Engine& engine) { (void)engine; }

    // Called when this scene becomes top again (after pop)
    virtual void onResume(Engine& engine) { (void)engine; }
};
