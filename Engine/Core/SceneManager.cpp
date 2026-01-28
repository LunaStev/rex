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
    pending.push_back(PendingOp{ PendingType::Replace, std::move(scene) });
}

void SceneManager::push(std::unique_ptr<Scene> scene) {
    pending.push_back(PendingOp{ PendingType::Push, std::move(scene) });
}

void SceneManager::pop() {
    pending.push_back(PendingOp{ PendingType::Pop, nullptr });
}

void SceneManager::applyPending(Engine& engine) {
    while (!pending.empty()) {
        PendingOp op = std::move(pending.front());
        pending.pop_front();

        switch (op.type) {
            case PendingType::Replace:
                doReplaceNow(engine, std::move(op.scene));
                break;
            case PendingType::Push:
                doPushNow(engine, std::move(op.scene));
                break;
            case PendingType::Pop:
                doPopNow(engine);
                break;
        }
    }
}

void SceneManager::clearNow(Engine& engine) {
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
    pending.clear();
}
