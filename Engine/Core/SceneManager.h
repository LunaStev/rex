#pragma once
#include <memory>
#include <vector>
#include <type_traits>
#include <utility>
#include <deque>

#include "Scene.h"

class Engine;
class Input;
class Graphics;

class SceneManager {
public:
    SceneManager() = default;

    Scene* current();
    const Scene* current() const;

    void replace(std::unique_ptr<Scene> scene);
    void push(std::unique_ptr<Scene> scene);
    void pop();

    template<typename T, typename... Args>
    void replace(Args&&... args) {
        static_assert(std::is_base_of_v<Scene, T>, "SceneManager::replace<T>: T must derive from Scene");
        replace(std::make_unique<T>(std::forward<Args>(args)...));
    }

    template<typename T, typename... Args>
    void push(Args&&... args) {
        static_assert(std::is_base_of_v<Scene, T>, "SceneManager::push<T>: T must derive from Scene");
        push(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void applyPending(Engine& engine);

    void updateTop(float dt, Input& input);
    void renderAll(Graphics& g);

    bool empty() const { return scenes.empty(); }
    void clear(Engine& engine);

private:
    enum class PendingType { Replace, Push, Pop };

    struct PendingOp {
        PendingType type;
        std::unique_ptr<Scene> scene; // Pop이면 null
    };

    std::vector<std::unique_ptr<Scene>> scenes;
    std::deque<PendingOp> pending;

    void clearNow(Engine& engine);

    void doReplaceNow(Engine& engine, std::unique_ptr<Scene> scene);
    void doPushNow(Engine& engine, std::unique_ptr<Scene> scene);
    void doPopNow(Engine& engine);
};
