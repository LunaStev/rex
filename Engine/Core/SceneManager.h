#pragma once
#include <memory>
#include <vector>
#include <type_traits>
#include <utility>

#include "Scene.h"

class Engine;   // forward
class Input;    // forward
class Graphics; // forward

class SceneManager {
public:
    SceneManager() = default;

    Scene* current();
    const Scene* current() const;

    // Schedule operations (applied in applyPending)
    void replace(std::unique_ptr<Scene> scene); // clear stack -> set new root
    void push(std::unique_ptr<Scene> scene);    // push new scene
    void pop();                                 // pop top scene

    // Convenience templates (Scene-derived only)
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
    void clear(Engine& engine); // public wrapper

private:
    enum class PendingType { None, Replace, Push, Pop };

    struct PendingOp {
        PendingType type = PendingType::None;
        std::unique_ptr<Scene> scene = nullptr;
    };

    std::vector<std::unique_ptr<Scene>> scenes;
    PendingOp pending;

    void clearNow(Engine& engine);

    void doReplaceNow(Engine& engine, std::unique_ptr<Scene> scene);
    void doPushNow(Engine& engine, std::unique_ptr<Scene> scene);
    void doPopNow(Engine& engine);
};
