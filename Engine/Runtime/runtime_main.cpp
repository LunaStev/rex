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
#include <limits>
#include <unordered_map>
#include <vector>

namespace {

using namespace rex;

constexpr float DEG2RAD = 0.01745329251994329577f;

inline float clampf(float v, float lo, float hi) {
    return std::clamp(v, lo, hi);
}

inline int signi(float v) {
    return (v > 0.0f) ? 1 : ((v < 0.0f) ? -1 : 0);
}

inline Vec3 forwardFromYawPitch(float yawDeg, float pitchDeg) {
    const float yaw = yawDeg * DEG2RAD;
    const float pitch = pitchDeg * DEG2RAD;
    const float cy = std::cos(yaw);
    const float sy = std::sin(yaw);
    const float cp = std::cos(pitch);
    const float sp = std::sin(pitch);
    return normalize(Vec3{sy * cp, sp, cy * cp});
}

inline Vec3 rightFromForward(const Vec3& forward) {
    return normalize(cross({0.0f, 1.0f, 0.0f}, forward));
}

inline Vec3 upFromForwardRight(const Vec3& forward, const Vec3& right) {
    return normalize(cross(forward, right));
}

struct GridPos {
    int x = 0;
    int y = 0;
    int z = 0;

    bool operator==(const GridPos& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
};

struct GridPosHash {
    size_t operator()(const GridPos& p) const noexcept {
        size_t h = static_cast<size_t>(std::hash<int>{}(p.x));
        h ^= static_cast<size_t>(std::hash<int>{}(p.y)) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= static_cast<size_t>(std::hash<int>{}(p.z)) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

enum class BlockKind {
    Grass,
    Dirt,
    Stone,
    Sand,
    Bedrock,
};

struct BlockVisual {
    Vec3 color{1.0f, 1.0f, 1.0f};
    float metallic = 0.02f;
    float roughness = 0.90f;
    float ao = 1.0f;
    const char* name = "Unknown";
};

BlockVisual getBlockVisual(BlockKind kind) {
    switch (kind) {
        case BlockKind::Grass: return {{0.34f, 0.65f, 0.28f}, 0.02f, 0.88f, 1.0f, "Grass"};
        case BlockKind::Dirt: return {{0.49f, 0.33f, 0.22f}, 0.01f, 0.92f, 1.0f, "Dirt"};
        case BlockKind::Stone: return {{0.56f, 0.57f, 0.61f}, 0.03f, 0.84f, 1.0f, "Stone"};
        case BlockKind::Sand: return {{0.77f, 0.72f, 0.48f}, 0.01f, 0.94f, 1.0f, "Sand"};
        case BlockKind::Bedrock: return {{0.14f, 0.14f, 0.16f}, 0.06f, 0.96f, 1.0f, "Bedrock"};
    }
    return {{1.0f, 0.0f, 1.0f}, 0.0f, 1.0f, 1.0f, "Invalid"};
}

const char* getBlockName(BlockKind kind) {
    return getBlockVisual(kind).name;
}

inline Vec3 toWorldPos(const GridPos& cell) {
    return {float(cell.x), float(cell.y), float(cell.z)};
}

int terrainHeight(int x, int z) {
    const float h = 4.5f
        + std::sin(float(x) * 0.22f) * 1.9f
        + std::cos(float(z) * 0.17f) * 1.5f
        + std::sin(float(x + z) * 0.11f) * 1.0f;
    return std::clamp(static_cast<int>(std::floor(h)), 2, 9);
}

using BlockMap = std::unordered_map<GridPos, EntityId, GridPosHash>;
using CellMap = std::unordered_map<EntityId, GridPos>;

struct GridRayHit {
    bool hit = false;
    GridPos cell{};
    GridPos normal{};
    float distance = 0.0f;
};

GridRayHit raycastBlocks(const Vec3& origin, const Vec3& direction, float maxDist, const BlockMap& blocks) {
    GridRayHit out{};
    if (maxDist <= 0.0f || blocks.empty()) return out;

    const Vec3 dir = normalize(direction);
    if (dot(dir, dir) <= 1e-8f) return out;

    // Convert to voxel grid where each block center is at integer coordinates.
    // Grid cell volume becomes [i-0.5, i+0.5), so shift ray by +0.5.
    const Vec3 p0{origin.x + 0.5f, origin.y + 0.5f, origin.z + 0.5f};

    GridPos cell{
        static_cast<int>(std::floor(p0.x)),
        static_cast<int>(std::floor(p0.y)),
        static_cast<int>(std::floor(p0.z))
    };

    auto hasCell = [&](const GridPos& c) -> bool {
        return blocks.find(c) != blocks.end();
    };

    if (hasCell(cell)) {
        out.hit = true;
        out.cell = cell;
        out.normal = {-signi(dir.x), -signi(dir.y), -signi(dir.z)};
        out.distance = 0.0f;
        return out;
    }

    const float inf = std::numeric_limits<float>::infinity();

    const int stepX = signi(dir.x);
    const int stepY = signi(dir.y);
    const int stepZ = signi(dir.z);

    const float tDeltaX = (stepX == 0) ? inf : std::fabs(1.0f / dir.x);
    const float tDeltaY = (stepY == 0) ? inf : std::fabs(1.0f / dir.y);
    const float tDeltaZ = (stepZ == 0) ? inf : std::fabs(1.0f / dir.z);

    float tMaxX = inf;
    float tMaxY = inf;
    float tMaxZ = inf;

    if (stepX != 0) {
        const float nextBoundary = float(cell.x + (stepX > 0 ? 1 : 0));
        tMaxX = (nextBoundary - p0.x) / dir.x;
    }
    if (stepY != 0) {
        const float nextBoundary = float(cell.y + (stepY > 0 ? 1 : 0));
        tMaxY = (nextBoundary - p0.y) / dir.y;
    }
    if (stepZ != 0) {
        const float nextBoundary = float(cell.z + (stepZ > 0 ? 1 : 0));
        tMaxZ = (nextBoundary - p0.z) / dir.z;
    }

    float traveled = 0.0f;
    GridPos enterNormal{0, 0, 0};

    while (traveled <= maxDist) {
        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                cell.x += stepX;
                traveled = tMaxX;
                tMaxX += tDeltaX;
                enterNormal = {-stepX, 0, 0};
            } else {
                cell.z += stepZ;
                traveled = tMaxZ;
                tMaxZ += tDeltaZ;
                enterNormal = {0, 0, -stepZ};
            }
        } else {
            if (tMaxY < tMaxZ) {
                cell.y += stepY;
                traveled = tMaxY;
                tMaxY += tDeltaY;
                enterNormal = {0, -stepY, 0};
            } else {
                cell.z += stepZ;
                traveled = tMaxZ;
                tMaxZ += tDeltaZ;
                enterNormal = {0, 0, -stepZ};
            }
        }

        if (traveled > maxDist) break;

        if (hasCell(cell)) {
            out.hit = true;
            out.cell = cell;
            out.normal = enterNormal;
            out.distance = traveled;
            return out;
        }
    }

    return out;
}

bool spawnBlock(Scene& scene,
                Mesh* cube,
                const GridPos& cell,
                BlockKind kind,
                BlockMap& blocks,
                CellMap& entityToCell) {
    if (blocks.find(cell) != blocks.end()) return false;

    const BlockVisual visual = getBlockVisual(kind);

    const EntityId e = scene.createEntity();
    Transform& t = scene.addComponent<Transform>(e, toWorldPos(cell));
    t.scale = {1.0f, 1.0f, 1.0f};

    scene.addComponent<MeshRenderer>(e, nullptr, cube, visual.color, visual.metallic, visual.roughness, visual.ao);

    blocks.emplace(cell, e);
    entityToCell.emplace(e, cell);
    return true;
}

bool removeBlock(Scene& scene,
                 EntityId e,
                 BlockMap& blocks,
                 CellMap& entityToCell) {
    auto it = entityToCell.find(e);
    if (it == entityToCell.end()) return false;

    blocks.erase(it->second);
    entityToCell.erase(it);
    scene.destroyEntity(e);
    return true;
}

EntityId spawnDynamicProp(Scene& scene,
                          Mesh* cube,
                          const Vec3& pos,
                          const Vec3& vel,
                          BlockKind kind) {
    const BlockVisual visual = getBlockVisual(kind);

    const EntityId e = scene.createEntity();
    Transform& t = scene.addComponent<Transform>(e, pos);
    t.scale = {1.0f, 1.0f, 1.0f};

    scene.addComponent<MeshRenderer>(e, nullptr, cube, visual.color, visual.metallic, visual.roughness, visual.ao);

    RigidBodyComponent& rb = scene.addComponent<RigidBodyComponent>(e, BodyType::Dynamic, 1.0f);
    rb.velocity = vel;
    rb.angularVelocity = {1.3f, 2.1f, 0.9f};
    rb.enableCCD = true;
    rb.restitution = 0.08f;
    rb.staticFriction = 0.75f;
    rb.dynamicFriction = 0.55f;
    rb.linearDamping = 0.01f;
    rb.angularDamping = 0.03f;

    return e;
}

} // namespace

int main() {
    using namespace rex;

    Window window({
        .title = "Rex Block Sandbox (Deferred + Rust Physics)",
        .width = 1600,
        .height = 900,
        .vsync = true,
    });

    Renderer renderer;
    auto& post = renderer.deferredPipeline().postProcess().settings();
    post.enableBloom = false;
    post.autoExposure = false;
    post.exposure = 1.05f;
    post.bloomStrength = 0.08f;

    PhysicsSystem physics;
    physics.setSolverIterations(12, 6);
    physics.setMaxSubSteps(8);
    physics.setGravity({0.0f, -9.81f, 0.0f});

    Scene scene;
    Mesh* cube = Mesh::createCube();

    Camera camera;
    camera.fov = 70.0f;
    camera.aspect = float(window.getWidth()) / float(window.getHeight());
    camera.nearPlane = 0.05f;
    camera.farPlane = 600.0f;

    Vec3 camPos{0.0f, 14.0f, -24.0f};
    float camYaw = 0.0f;
    float camPitch = -14.0f;

    bool running = true;
    bool paused = false;
    bool gravityEnabled = true;
    bool lookMode = false;
    float worldTime = 0.0f;

    BlockKind selectedBlock = BlockKind::Grass;

    BlockMap blocks;
    CellMap entityToCell;
    std::vector<EntityId> dynamicProps;

    const EntityId sun = scene.createEntity();
    scene.addComponent<Transform>(sun, Vec3{0.0f, 80.0f, 0.0f}).rotation = {-54.0f, 22.0f, 0.0f};
    Light& sunLight = scene.addComponent<Light>(sun, Vec3{1.0f, 0.97f, 0.93f}, 4.6f, Light::Directional);
    sunLight.castShadows = true;
    sunLight.volumetric = false;

    const EntityId skyFill = scene.createEntity();
    scene.addComponent<Transform>(skyFill, Vec3{0.0f, 40.0f, 0.0f}).rotation = {32.0f, -140.0f, 0.0f};
    Light& fill = scene.addComponent<Light>(skyFill, Vec3{0.42f, 0.50f, 0.62f}, 0.8f, Light::Directional);
    fill.castShadows = false;

    const int worldHalf = 14;
    int spawnedBlocks = 0;

    for (int x = -worldHalf; x <= worldHalf; ++x) {
        for (int z = -worldHalf; z <= worldHalf; ++z) {
            const int h = terrainHeight(x, z);
            for (int y = 0; y <= h; ++y) {
                BlockKind kind = BlockKind::Stone;
                if (y == 0) {
                    kind = BlockKind::Bedrock;
                } else if (y == h) {
                    kind = (h <= 3) ? BlockKind::Sand : BlockKind::Grass;
                } else if (y >= h - 2) {
                    kind = BlockKind::Dirt;
                }

                if (spawnBlock(scene, cube, {x, y, z}, kind, blocks, entityToCell)) {
                    ++spawnedBlocks;
                }
            }
        }
    }

    for (int i = 0; i < 14; ++i) {
        const int x = -worldHalf + 2 + (i * 7) % (worldHalf * 2 - 3);
        const int z = -worldHalf + 2 + (i * 11) % (worldHalf * 2 - 3);
        const int baseH = terrainHeight(x, z);
        const int towerH = 3 + (i % 4);
        for (int y = baseH + 1; y <= baseH + towerH; ++y) {
            if (spawnBlock(scene, cube, {x, y, z}, BlockKind::Stone, blocks, entityToCell)) {
                ++spawnedBlocks;
            }
        }
    }

    Logger::info("Rex Block Sandbox ready. spawned blocks: {}", spawnedBlocks);
    Logger::info("Controls:");
    Logger::info("WASD + QE: move camera, Shift: speed boost");
    Logger::info("RMB hold + mouse: free look");
    Logger::info("LMB: break block, RMB click: place selected block");
    Logger::info("1/2/3/4: select block (Grass/Dirt/Stone/Sand)");
    Logger::info("F: throw dynamic block prop");
    Logger::info("P: pause physics, G: toggle gravity");
    Logger::info("F1/F2/F3/F4/F5/F6: post-process tuning");
    Logger::info("Delete: remove last spawned prop, Esc: quit");

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
                constexpr float mouseSensitivity = 0.085f;
                camYaw += float(e.motion.xrel) * mouseSensitivity;
                camPitch = clampf(camPitch - float(e.motion.yrel) * mouseSensitivity, -86.0f, 86.0f);
            } else if (e.type == SDL_MOUSEBUTTONDOWN && (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT)) {
                const Vec3 forward = forwardFromYawPitch(camYaw, camPitch);
                const GridRayHit blockHit = raycastBlocks(camPos, forward, 14.0f, blocks);

                if (e.button.button == SDL_BUTTON_LEFT) {
                    if (blockHit.hit) {
                        if (blockHit.cell.y <= 0) {
                            Logger::trace("Bedrock block is protected");
                        } else {
                            auto it = blocks.find(blockHit.cell);
                            if (it != blocks.end()) {
                                removeBlock(scene, it->second, blocks, entityToCell);
                            }
                        }
                    } else {
                        const RaycastHit bodyHit = physics.raycast(camPos, forward, 14.0f);
                        if (bodyHit.hit && bodyHit.body) {
                            bodyHit.body->applyImpulse(forward * 10.0f);
                        }
                    }
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    if (!blockHit.hit) continue;

                    const GridPos target{
                        blockHit.cell.x + blockHit.normal.x,
                        blockHit.cell.y + blockHit.normal.y,
                        blockHit.cell.z + blockHit.normal.z
                    };

                    const bool inBounds =
                        (target.x >= -worldHalf * 2 && target.x <= worldHalf * 2) &&
                        (target.z >= -worldHalf * 2 && target.z <= worldHalf * 2) &&
                        (target.y >= 1 && target.y <= 42);
                    if (!inBounds) continue;

                    spawnBlock(scene, cube, target, selectedBlock, blocks, entityToCell);
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
                    case SDLK_1:
                        selectedBlock = BlockKind::Grass;
                        Logger::info("Selected block: {}", getBlockName(selectedBlock));
                        break;
                    case SDLK_2:
                        selectedBlock = BlockKind::Dirt;
                        Logger::info("Selected block: {}", getBlockName(selectedBlock));
                        break;
                    case SDLK_3:
                        selectedBlock = BlockKind::Stone;
                        Logger::info("Selected block: {}", getBlockName(selectedBlock));
                        break;
                    case SDLK_4:
                        selectedBlock = BlockKind::Sand;
                        Logger::info("Selected block: {}", getBlockName(selectedBlock));
                        break;
                    case SDLK_f: {
                        const Vec3 forward = forwardFromYawPitch(camYaw, camPitch);
                        const EntityId prop = spawnDynamicProp(
                            scene,
                            cube,
                            camPos + forward * 1.7f + Vec3{0.0f, 0.5f, 0.0f},
                            forward * 16.0f + Vec3{0.0f, 2.0f, 0.0f},
                            selectedBlock);
                        dynamicProps.push_back(prop);
                        break;
                    }
                    case SDLK_DELETE: {
                        if (!dynamicProps.empty()) {
                            const EntityId id = dynamicProps.back();
                            dynamicProps.pop_back();
                            scene.destroyEntity(id);
                        }
                        break;
                    }
                    case SDLK_F1:
                        post.enableBloom = !post.enableBloom;
                        Logger::info("Bloom {}", post.enableBloom ? "enabled" : "disabled");
                        break;
                    case SDLK_F2:
                        post.autoExposure = !post.autoExposure;
                        Logger::info("Auto Exposure {}", post.autoExposure ? "enabled" : "disabled");
                        break;
                    case SDLK_F3:
                        post.exposure = clampf(post.exposure - 0.08f, 0.2f, 4.0f);
                        Logger::info("Exposure {:.2f}", post.exposure);
                        break;
                    case SDLK_F4:
                        post.exposure = clampf(post.exposure + 0.08f, 0.2f, 4.0f);
                        Logger::info("Exposure {:.2f}", post.exposure);
                        break;
                    case SDLK_F5:
                        post.bloomStrength = clampf(post.bloomStrength - 0.02f, 0.0f, 1.0f);
                        Logger::info("Bloom Strength {:.2f}", post.bloomStrength);
                        break;
                    case SDLK_F6:
                        post.bloomStrength = clampf(post.bloomStrength + 0.02f, 0.0f, 1.0f);
                        Logger::info("Bloom Strength {:.2f}", post.bloomStrength);
                        break;
                    default:
                        break;
                }
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        const Vec3 forward = forwardFromYawPitch(camYaw, camPitch);
        const Vec3 right = rightFromForward(forward);
        const Vec3 up = upFromForwardRight(forward, right);

        float moveSpeed = 10.0f;
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

        if (auto* t = scene.getComponent<Transform>(sun)) {
            t->rotation.x = -54.0f + std::sin(worldTime * 0.05f) * 6.0f;
            t->rotation.y = 22.0f + std::cos(worldTime * 0.04f) * 14.0f;
        }

        if (auto* t = scene.getComponent<Transform>(skyFill)) {
            t->rotation.x = 32.0f + std::sin(worldTime * 0.07f) * 4.0f;
            t->rotation.y = -140.0f + std::cos(worldTime * 0.03f) * 12.0f;
        }

        physics.update(scene, paused ? 0.0f : dt);

        const Mat4 view = Mat4::lookAtLH(camPos, camPos + forward, {0.0f, 1.0f, 0.0f});
        renderer.render(scene, camera, view, camPos, window.getWidth(), window.getHeight(), 0);
        window.swapBuffers();
    }

    SDL_SetRelativeMouseMode(SDL_FALSE);
    delete cube;
    return 0;
}
