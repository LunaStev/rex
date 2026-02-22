#include "../Core/Components.h"
#include "../Core/Logger.h"
#include "../Core/Scene.h"
#include "../Core/Window.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Renderer.h"
#include "../Physics/PhysicsSystem.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace {

constexpr float DEG2RAD = 0.01745329251994329577f;

inline float clampf(float v, float lo, float hi) {
    return std::clamp(v, lo, hi);
}

inline rex::Vec3 forwardFromYawPitch(float yawDeg, float pitchDeg) {
    const float yaw = yawDeg * DEG2RAD;
    const float pitch = pitchDeg * DEG2RAD;
    const float cy = std::cos(yaw);
    const float sy = std::sin(yaw);
    const float cp = std::cos(pitch);
    const float sp = std::sin(pitch);
    const rex::Vec3 forward{sy * cp, sp, cy * cp};
    return rex::normalize(forward);
}

inline rex::Vec3 rightFromForward(const rex::Vec3& forward) {
    return rex::normalize(rex::cross({0.0f, 1.0f, 0.0f}, forward));
}

inline rex::Vec3 upFromForwardRight(const rex::Vec3& forward, const rex::Vec3& right) {
    return rex::normalize(rex::cross(forward, right));
}

rex::EntityId spawnCube(
    rex::Scene& scene,
    rex::Mesh* cube,
    const rex::Vec3& position,
    const rex::Vec3& scale,
    const rex::Vec3& color,
    rex::BodyType bodyType,
    float mass,
    const rex::Vec3& velocity = {0, 0, 0},
    const rex::Vec3& angularVelocity = {0, 0, 0},
    bool ccd = true,
    float restitution = 0.25f,
    float staticFriction = 0.7f,
    float dynamicFriction = 0.5f,
    float metallic = 0.05f,
    float roughness = 0.65f,
    float ao = 1.0f) {

    using namespace rex;

    const EntityId entity = scene.createEntity();

    Transform& t = scene.addComponent<Transform>(entity, position);
    t.scale = scale;

    scene.addComponent<MeshRenderer>(entity, nullptr, cube, color, metallic, roughness, ao);

    RigidBodyComponent& rb = scene.addComponent<RigidBodyComponent>(entity, bodyType, mass);
    rb.enableCCD = ccd;
    rb.velocity = velocity;
    rb.angularVelocity = angularVelocity;
    rb.restitution = restitution;
    rb.staticFriction = staticFriction;
    rb.dynamicFriction = dynamicFriction;
    rb.linearDamping = 0.02f;
    rb.angularDamping = 0.06f;

    return entity;
}

rex::EntityId spawnLightEntity(
    rex::Scene& scene,
    const rex::Vec3& position,
    const rex::Vec3& rotationDeg,
    const rex::Vec3& color,
    float intensity,
    rex::Light::Type type,
    float range,
    bool castShadows,
    bool volumetric,
    float innerConeDeg = 20.0f,
    float outerConeDeg = 35.0f,
    float attenuationLinear = 0.09f,
    float attenuationQuadratic = 0.032f) {
    using namespace rex;

    const EntityId entity = scene.createEntity();
    Transform& t = scene.addComponent<Transform>(entity, position);
    t.rotation = rotationDeg;

    Light& light = scene.addComponent<Light>(entity, color, intensity, type);
    light.range = range;
    light.castShadows = castShadows;
    light.volumetric = volumetric;
    light.innerConeDeg = innerConeDeg;
    light.outerConeDeg = outerConeDeg;
    light.attenuationLinear = attenuationLinear;
    light.attenuationQuadratic = attenuationQuadratic;
    return entity;
}

void connectChain(
    rex::PhysicsSystem& physics,
    rex::Scene& scene,
    const std::vector<rex::EntityId>& chain,
    std::vector<int>& outJointIds,
    float stiffness,
    float damping) {

    using namespace rex;

    for (size_t i = 1; i < chain.size(); ++i) {
        auto* a = scene.getComponent<RigidBodyComponent>(chain[i - 1]);
        auto* b = scene.getComponent<RigidBodyComponent>(chain[i]);
        if (!a || !b || !a->internalBody || !b->internalBody) {
            continue;
        }

        DistanceJointDesc j;
        j.bodyA = a->internalBody;
        j.bodyB = b->internalBody;
        j.restLength = 1.45f;
        j.stiffness = stiffness;
        j.damping = damping;
        const int id = physics.addDistanceJoint(j);
        if (id >= 0) {
            outJointIds.push_back(id);
        }
    }
}

} // namespace

int main() {
    using namespace rex;

    Window window({
        .title = "Rex Runtime Sandbox (Graphics + Rust Physics)",
        .width = 1600,
        .height = 900,
        .vsync = true,
    });

    Renderer renderer;
    auto& postSettings = renderer.deferredPipeline().postProcess().settings();
    postSettings.enableBloom = true;
    postSettings.autoExposure = true;
    postSettings.exposure = 1.15f;
    postSettings.bloomStrength = 0.14f;

    PhysicsSystem physics;
    physics.setSolverIterations(14, 6);
    physics.setMaxSubSteps(8);
    physics.setGravity({0.0f, -9.81f, 0.0f});

    Mesh* cube = Mesh::createCube();
    Scene scene;

    Camera camera;
    camera.fov = 65.0f;
    camera.aspect = float(window.getWidth()) / float(window.getHeight());
    camera.nearPlane = 0.05f;
    camera.farPlane = 2000.0f;

    Vec3 camPos{0.0f, 4.0f, -16.0f};
    float camYaw = 0.0f;
    float camPitch = 10.0f;

    bool running = true;
    bool paused = false;
    bool gravityEnabled = true;
    bool lookMode = false;

    float worldTime = 0.0f;
    Vec3 kinematicLastPos{0, 0, 0};

    std::vector<EntityId> dynamicBodies;
    std::vector<EntityId> chainBodies;
    std::vector<int> chainJointIds;
    std::vector<EntityId> stressPointLights;
    std::vector<Vec3> stressPointLightBase;

    bool stressLightsEnabled = true;
    bool animateStressLights = true;
    float stressLightIntensity = 2.5f;

    const EntityId sunLight = spawnLightEntity(
        scene,
        {0.0f, 40.0f, 0.0f},
        {-48.0f, 38.0f, 0.0f},
        {1.0f, 0.97f, 0.93f},
        6.8f,
        Light::Directional,
        1500.0f,
        true,
        false);

    const EntityId movingPointLight = spawnLightEntity(
        scene,
        {6.0f, 8.0f, -2.0f},
        {0.0f, 0.0f, 0.0f},
        {0.78f, 0.88f, 1.0f},
        12.0f,
        Light::Point,
        35.0f,
        false,
        false,
        20.0f,
        35.0f,
        0.05f,
        0.02f);

    const EntityId movingSpotLight = spawnLightEntity(
        scene,
        {-4.0f, 7.0f, 4.0f},
        {-40.0f, 20.0f, 0.0f},
        {1.0f, 0.82f, 0.60f},
        18.0f,
        Light::Spot,
        42.0f,
        false,
        false,
        18.0f,
        34.0f,
        0.06f,
        0.016f);

    for (int i = 0; i < 8; ++i) {
        const float angle = (float(i) / 8.0f) * 6.28318530718f;
        const Vec3 pos{std::sin(angle) * 16.0f, 5.5f, std::cos(angle) * 16.0f};
        const Vec3 color{
            0.45f + 0.45f * (0.5f + 0.5f * std::sin(angle + 0.3f)),
            0.40f + 0.50f * (0.5f + 0.5f * std::sin(angle + 2.1f)),
            0.55f + 0.35f * (0.5f + 0.5f * std::sin(angle + 4.2f))
        };
        spawnLightEntity(scene, pos, {-30.0f, angle * 57.2957795f, 0.0f}, color, 6.0f, Light::Area, 18.0f, false, false);
    }

    constexpr int stressGridX = 16;
    constexpr int stressGridZ = 8;
    for (int z = 0; z < stressGridZ; ++z) {
        for (int x = 0; x < stressGridX; ++x) {
            const float fx = -14.0f + float(x) * 1.85f;
            const float fz = -14.0f + float(z) * 3.45f;
            const Vec3 base{fx, 1.4f, fz};
            const float phase = float(x + z * stressGridX) * 0.13f;
            const Vec3 color{
                0.35f + 0.65f * (0.5f + 0.5f * std::sin(phase + 0.8f)),
                0.35f + 0.65f * (0.5f + 0.5f * std::sin(phase + 2.8f)),
                0.35f + 0.65f * (0.5f + 0.5f * std::sin(phase + 4.6f))
            };
            const EntityId lightId = spawnLightEntity(
                scene,
                base,
                {0.0f, 0.0f, 0.0f},
                color,
                stressLightIntensity,
                Light::Point,
                8.0f,
                false,
                false,
                20.0f,
                35.0f,
                0.16f,
                0.11f);
            stressPointLights.push_back(lightId);
            stressPointLightBase.push_back(base);
        }
    }

    spawnCube(scene, cube, {0.0f, -2.0f, 0.0f}, {36.0f, 1.0f, 36.0f}, {0.45f, 0.45f, 0.48f}, BodyType::Static, 0.0f, {0,0,0}, {0,0,0}, false, 0.05f, 0.9f, 0.65f, 0.02f, 0.82f, 1.0f);
    spawnCube(scene, cube, {0.0f, 6.0f, 18.0f}, {36.0f, 16.0f, 1.0f}, {0.25f, 0.28f, 0.34f}, BodyType::Static, 0.0f, {0,0,0}, {0,0,0}, false, 0.05f, 0.9f, 0.65f, 0.10f, 0.75f, 1.0f);
    spawnCube(scene, cube, {-18.0f, 6.0f, 0.0f}, {1.0f, 16.0f, 36.0f}, {0.25f, 0.28f, 0.34f}, BodyType::Static, 0.0f, {0,0,0}, {0,0,0}, false, 0.05f, 0.9f, 0.65f, 0.10f, 0.75f, 1.0f);
    spawnCube(scene, cube, {18.0f, 6.0f, 0.0f}, {1.0f, 16.0f, 36.0f}, {0.25f, 0.28f, 0.34f}, BodyType::Static, 0.0f, {0,0,0}, {0,0,0}, false, 0.05f, 0.9f, 0.65f, 0.10f, 0.75f, 1.0f);

    const EntityId kinematicPlatform = spawnCube(
        scene,
        cube,
        {0.0f, 2.0f, -4.0f},
        {4.0f, 0.6f, 4.0f},
        {0.25f, 0.65f, 0.9f},
        BodyType::Kinematic,
        0.0f,
        {0,0,0},
        {0,0,0},
        false,
        0.05f,
        0.8f,
        0.6f);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 3; ++k) {
                const Vec3 pos{-3.0f + i * 1.35f, 0.4f + k * 1.2f, -8.0f + j * 1.35f};
                const Vec3 color{0.65f + 0.08f * float(k), 0.35f + 0.05f * float(i), 0.30f + 0.06f * float(j)};
                dynamicBodies.push_back(spawnCube(scene, cube, pos, {1.0f, 1.0f, 1.0f}, color, BodyType::Dynamic, 1.0f));
            }
        }
    }

    for (int i = 0; i < 8; ++i) {
        const Vec3 pos{-12.0f + float(i) * 1.5f, 9.5f, -12.0f};
        chainBodies.push_back(spawnCube(
            scene,
            cube,
            pos,
            {0.9f, 0.9f, 0.9f},
            {0.9f, 0.8f, 0.25f},
            BodyType::Dynamic,
            0.7f,
            {0,0,0},
            {0,0.7f,0}));
    }

    physics.update(scene, 0.0f);

    connectChain(physics, scene, chainBodies, chainJointIds, 0.72f, 0.18f);

    if (auto* t = scene.getComponent<Transform>(kinematicPlatform)) {
        kinematicLastPos = t->position;
    }

    Logger::info("Runtime Sandbox Controls:");
    Logger::info("WASD + QE: move camera, Shift: speed boost");
    Logger::info("Right mouse hold: look around");
    Logger::info("Left click: raycast + impulse at hit point");
    Logger::info("Space: spawn projectile cube");
    Logger::info("B: spawn drop cube, J: connect last 2 cubes with joint");
    Logger::info("R: impulse burst, T: torque burst, G: toggle gravity, P: pause");
    Logger::info("F1: bloom toggle, F2: auto exposure toggle, F3/F4: exposure -, +");
    Logger::info("F5/F6: bloom strength -, +, L: stress lights on/off, K: stress animation toggle");
    Logger::info("Delete: remove last spawned dynamic cube, Esc: quit");

    uint64_t prevCounter = SDL_GetPerformanceCounter();
    const uint64_t perfFreq = SDL_GetPerformanceFrequency();

    while (running) {
        const uint64_t now = SDL_GetPerformanceCounter();
        float dt = float(now - prevCounter) / float(perfFreq);
        prevCounter = now;
        dt = clampf(dt, 1.0f / 240.0f, 1.0f / 20.0f);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                const int w = std::max(1, e.window.data1);
                const int h = std::max(1, e.window.data2);
                camera.aspect = float(w) / float(h);
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT) {
                lookMode = true;
                SDL_SetRelativeMouseMode(SDL_TRUE);
            } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT) {
                lookMode = false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
            } else if (e.type == SDL_MOUSEMOTION && lookMode) {
                constexpr float mouseSensitivity = 0.08f;
                camYaw += float(e.motion.xrel) * mouseSensitivity;
                camPitch = clampf(camPitch - float(e.motion.yrel) * mouseSensitivity, -85.0f, 85.0f);
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                const Vec3 forward = forwardFromYawPitch(camYaw, camPitch);
                const RaycastHit hit = physics.raycast(camPos, forward, 200.0f);
                if (hit.hit && hit.body) {
                    hit.body->applyImpulseAtPoint(forward * 32.0f, hit.point);
                    Logger::info("Raycast hit at ({:.2f}, {:.2f}, {:.2f}), dist {:.2f}", hit.point.x, hit.point.y, hit.point.z, hit.distance);
                } else {
                    Logger::trace("Raycast miss");
                }
            } else if (e.type == SDL_KEYDOWN && !e.key.repeat) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_p:
                        paused = !paused;
                        Logger::info("Physics {}", paused ? "paused" : "running");
                        break;
                    case SDLK_g:
                        gravityEnabled = !gravityEnabled;
                        physics.setGravity(gravityEnabled ? Vec3{0.0f, -9.81f, 0.0f} : Vec3{0.0f, 0.0f, 0.0f});
                        Logger::info("Gravity {}", gravityEnabled ? "enabled" : "disabled");
                        break;
                    case SDLK_F1:
                        postSettings.enableBloom = !postSettings.enableBloom;
                        Logger::info("Bloom {}", postSettings.enableBloom ? "enabled" : "disabled");
                        break;
                    case SDLK_F2:
                        postSettings.autoExposure = !postSettings.autoExposure;
                        Logger::info("Auto Exposure {}", postSettings.autoExposure ? "enabled" : "disabled");
                        break;
                    case SDLK_F3:
                        postSettings.exposure = clampf(postSettings.exposure - 0.1f, 0.2f, 4.0f);
                        Logger::info("Exposure {:.2f}", postSettings.exposure);
                        break;
                    case SDLK_F4:
                        postSettings.exposure = clampf(postSettings.exposure + 0.1f, 0.2f, 4.0f);
                        Logger::info("Exposure {:.2f}", postSettings.exposure);
                        break;
                    case SDLK_F5:
                        postSettings.bloomStrength = clampf(postSettings.bloomStrength - 0.02f, 0.0f, 1.0f);
                        Logger::info("Bloom Strength {:.2f}", postSettings.bloomStrength);
                        break;
                    case SDLK_F6:
                        postSettings.bloomStrength = clampf(postSettings.bloomStrength + 0.02f, 0.0f, 1.0f);
                        Logger::info("Bloom Strength {:.2f}", postSettings.bloomStrength);
                        break;
                    case SDLK_l:
                        stressLightsEnabled = !stressLightsEnabled;
                        for (EntityId id : stressPointLights) {
                            auto* l = scene.getComponent<Light>(id);
                            if (!l) continue;
                            l->intensity = stressLightsEnabled ? stressLightIntensity : 0.0f;
                        }
                        Logger::info("Stress point lights {}", stressLightsEnabled ? "enabled" : "disabled");
                        break;
                    case SDLK_k:
                        animateStressLights = !animateStressLights;
                        Logger::info("Stress light animation {}", animateStressLights ? "enabled" : "disabled");
                        break;
                    case SDLK_SPACE: {
                        const Vec3 forward = forwardFromYawPitch(camYaw, camPitch);
                        const EntityId bullet = spawnCube(
                            scene,
                            cube,
                            camPos + forward * 1.7f,
                            {0.45f, 0.45f, 0.45f},
                            {0.9f, 0.2f, 0.2f},
                            BodyType::Dynamic,
                            0.5f,
                            forward * 28.0f,
                            {0, 4.0f, 0},
                            true,
                            0.15f,
                            0.4f,
                            0.3f);
                        dynamicBodies.push_back(bullet);
                        break;
                    }
                    case SDLK_b: {
                        const Vec3 forward = forwardFromYawPitch(camYaw, camPitch);
                        const Vec3 right = rightFromForward(forward);
                        const EntityId spawned = spawnCube(
                            scene,
                            cube,
                            camPos + forward * 5.0f + right * 1.2f + Vec3{0, 3, 0},
                            {1.0f, 1.0f, 1.0f},
                            {0.2f, 0.7f, 0.4f},
                            BodyType::Dynamic,
                            1.0f,
                            {0, 0, 0},
                            {0.5f, 0.5f, 0},
                            true,
                            0.28f,
                            0.7f,
                            0.45f);
                        dynamicBodies.push_back(spawned);
                        break;
                    }
                    case SDLK_j: {
                        if (dynamicBodies.size() >= 2) {
                            auto* rbA = scene.getComponent<RigidBodyComponent>(dynamicBodies[dynamicBodies.size() - 2]);
                            auto* rbB = scene.getComponent<RigidBodyComponent>(dynamicBodies.back());
                            if (rbA && rbB && rbA->internalBody && rbB->internalBody) {
                                DistanceJointDesc j;
                                j.bodyA = rbA->internalBody;
                                j.bodyB = rbB->internalBody;
                                j.restLength = 1.8f;
                                j.stiffness = 0.8f;
                                j.damping = 0.2f;
                                const int id = physics.addDistanceJoint(j);
                                if (id >= 0) {
                                    chainJointIds.push_back(id);
                                    Logger::info("Added runtime joint {}", id);
                                }
                            }
                        }
                        break;
                    }
                    case SDLK_r: {
                        for (EntityId id : dynamicBodies) {
                            auto* rb = scene.getComponent<RigidBodyComponent>(id);
                            if (rb && rb->internalBody) {
                                rb->internalBody->applyImpulse({0.0f, 7.5f, 0.0f});
                            }
                        }
                        Logger::info("Impulse burst applied to dynamic set");
                        break;
                    }
                    case SDLK_t: {
                        for (EntityId id : dynamicBodies) {
                            auto* rb = scene.getComponent<RigidBodyComponent>(id);
                            if (rb && rb->internalBody) {
                                rb->internalBody->applyTorque({0.0f, 10.0f, 6.0f});
                            }
                        }
                        Logger::info("Torque burst applied to dynamic set");
                        break;
                    }
                    case SDLK_DELETE: {
                        if (!dynamicBodies.empty()) {
                            const EntityId id = dynamicBodies.back();
                            dynamicBodies.pop_back();
                            scene.destroyEntity(id);
                            Logger::info("Destroyed entity {}", id);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        const Vec3 forward = forwardFromYawPitch(camYaw, camPitch);
        const Vec3 right = rightFromForward(forward);
        const Vec3 up = upFromForwardRight(forward, right);

        float moveSpeed = 9.0f;
        if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) {
            moveSpeed *= 2.0f;
        }

        if (keys[SDL_SCANCODE_W]) camPos += forward * (moveSpeed * dt);
        if (keys[SDL_SCANCODE_S]) camPos -= forward * (moveSpeed * dt);
        if (keys[SDL_SCANCODE_D]) camPos += right * (moveSpeed * dt);
        if (keys[SDL_SCANCODE_A]) camPos -= right * (moveSpeed * dt);
        if (keys[SDL_SCANCODE_E]) camPos += up * (moveSpeed * dt);
        if (keys[SDL_SCANCODE_Q]) camPos -= up * (moveSpeed * dt);

        worldTime += dt;

        if (auto* t = scene.getComponent<Transform>(sunLight)) {
            t->rotation.x = -48.0f + std::sin(worldTime * 0.11f) * 4.0f;
            t->rotation.y = 38.0f + std::sin(worldTime * 0.09f) * 18.0f;
        }

        if (auto* t = scene.getComponent<Transform>(movingPointLight)) {
            t->position = {
                std::sin(worldTime * 0.7f) * 10.0f,
                8.0f + std::sin(worldTime * 1.9f) * 1.8f,
                -2.0f + std::cos(worldTime * 0.6f) * 8.0f
            };
        }

        if (auto* l = scene.getComponent<Light>(movingPointLight)) {
            l->intensity = 11.5f + std::sin(worldTime * 3.2f) * 2.2f;
        }

        if (auto* t = scene.getComponent<Transform>(movingSpotLight)) {
            t->position = {std::cos(worldTime * 0.55f) * 9.0f, 9.5f, std::sin(worldTime * 0.55f) * 9.0f};
            t->rotation = {-42.0f, worldTime * 31.0f, 0.0f};
        }

        if (auto* l = scene.getComponent<Light>(movingSpotLight)) {
            l->intensity = 16.0f + std::sin(worldTime * 2.5f) * 3.0f;
        }

        for (size_t i = 0; i < stressPointLights.size(); ++i) {
            const EntityId id = stressPointLights[i];
            auto* light = scene.getComponent<Light>(id);
            auto* t = scene.getComponent<Transform>(id);
            if (!light || !t) continue;

            light->intensity = stressLightsEnabled ? stressLightIntensity : 0.0f;
            if (animateStressLights) {
                const Vec3 base = stressPointLightBase[i];
                const float phase = float(i) * 0.27f;
                t->position = {base.x, base.y + std::sin(worldTime * 1.5f + phase) * 0.45f, base.z};
            } else {
                t->position = stressPointLightBase[i];
            }
        }

        if (auto* t = scene.getComponent<Transform>(kinematicPlatform)) {
            auto* rb = scene.getComponent<RigidBodyComponent>(kinematicPlatform);
            const Vec3 nextPos{std::sin(worldTime * 1.2f) * 6.0f, 2.0f + std::sin(worldTime * 2.1f) * 0.8f, -4.0f + std::cos(worldTime * 0.9f) * 3.5f};
            if (rb && dt > 0.0f) {
                rb->velocity = (nextPos - kinematicLastPos) * (1.0f / dt);
                rb->angularVelocity = {0.0f, 0.35f, 0.0f};
            }
            t->position = nextPos;
            t->rotation.y += 20.0f * dt;
            kinematicLastPos = nextPos;
        }

        if (!paused) {
            physics.update(scene, dt);
        }

        const Mat4 view = Mat4::lookAtLH(camPos, camPos + forward, {0, 1, 0});
        renderer.render(scene, camera, view, camPos, window.getWidth(), window.getHeight(), 0);
        window.swapBuffers();
    }

    SDL_SetRelativeMouseMode(SDL_FALSE);

    delete cube;
    return 0;
}
