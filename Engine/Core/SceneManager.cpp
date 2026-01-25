#include "SceneManager.h"
#include "Engine.h"

Scene* SceneManager::current() {
    if (scenes.empty()) return nullptr;
    return scenes.back().get();
}

const Scene* SceneManager::current() const {
    if (scenes.empty()) return nullptr;
    return scenes.back().get();
}

void SceneManager::replace(std::unique_ptr<Scene> scene) {
    pending.type = PendingType::Replace;
    pending.scene = std::move(scene);
}

void SceneManager::push(std::unique_ptr<Scene> scene) {
    pending.type = PendingType::Push;
    pending.scene = std::move(scene);
}

void SceneManager::pop() {
    pending.type = PendingType::Pop;
    pending.scene.reset();
}

void SceneManager::applyPending(Engine& engine) {
    if (pending.type == PendingType::None) return;

    switch (pending.type) {
        case PendingType::Replace:
            doReplaceNow(engine, std::move(pending.scene));
            break;
        case PendingType::Push:
            doPushNow(engine, std::move(pending.scene));
            break;
        case PendingType::Pop:
            doPopNow(engine);
            break;
        default:
            break;
    }

    pending.type = PendingType::None;
    pending.scene.reset();
}

void SceneManager::clearNow(Engine& engine) {
    // Exit from top to bottom
    while (!scenes.empty()) {
        scenes.back()->onExit(engine);
        scenes.pop_back();
    }
}

void SceneManager::doReplaceNow(Engine& engine, std::unique_ptr<Scene> scene) {
    clearNow(engine);
    if (!scene) return;

    scenes.push_back(std::move(scene));
    scenes.back()->onEnter(engine);
}

void SceneManager::doPushNow(Engine& engine, std::unique_ptr<Scene> scene) {
    if (!scene) return;

    if (!scenes.empty()) {
        scenes.back()->onPause(engine);
    }

    scenes.push_back(std::move(scene));
    scenes.back()->onEnter(engine);
}

void SceneManager::doPopNow(Engine& engine) {
    if (scenes.empty()) return;

    scenes.back()->onExit(engine);
    scenes.pop_back();

    if (!scenes.empty()) {
        scenes.back()->onResume(engine);
    } else {
        // No scene left -> stop engine loop
        engine.stop();
    }
}

void SceneManager::updateTop(float dt, Input& input) {
    Scene* s = current();
    if (!s) return;
    s->update(dt, input);
}

void SceneManager::renderAll(Graphics& g) {
    for (auto& s : scenes) {
        s->render(g);
    }
}

void SceneManager::clear(Engine& engine) {
    clearNow(engine);
    pending.type = PendingType::None;
    pending.scene.reset();
}
