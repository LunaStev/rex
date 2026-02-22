#include "PhysicsWorld.h"
#include "RigidBody.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <unordered_set>

namespace rex {

namespace {
inline float absf(float v) {
    return v < 0.0f ? -v : v;
}

inline float lengthSq(const Vec3& v) {
    return dot(v, v);
}

inline float min3(float a, float b, float c) {
    return std::min(a, std::min(b, c));
}

inline float clamp01(float v) {
    return std::clamp(v, 0.0f, 1.0f);
}

inline float getComp(const Vec3& v, int i) {
    return i == 0 ? v.x : (i == 1 ? v.y : v.z);
}

inline Vec3 safeNormalize(const Vec3& v) {
    const float lenSq = lengthSq(v);
    if (lenSq <= 1e-8f) return {0,0,0};
    const float invLen = 1.0f / std::sqrt(lenSq);
    return { v.x * invLen, v.y * invLen, v.z * invLen };
}

inline Vec3 pickPerpendicular(const Vec3& n) {
    Vec3 base = absf(n.x) < 0.577f ? Vec3{1,0,0} : Vec3{0,1,0};
    Vec3 t = cross(base, n);
    if (lengthSq(t) <= 1e-8f) {
        base = {0,0,1};
        t = cross(base, n);
    }
    return safeNormalize(t);
}

inline Quat integrateOrientation(const Quat& q, const Vec3& omega, float dt) {
    const Quat omegaQ{omega.x, omega.y, omega.z, 0.0f};
    const Quat dq = omegaQ * q;
    return normalize(q + dq * (0.5f * dt));
}

template <typename ObbT>
inline std::array<Vec3, 8> computeObbVertices(const ObbT& obb) {
    std::array<Vec3, 8> verts{};
    int idx = 0;
    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            for (int z = -1; z <= 1; z += 2) {
                verts[idx++] = obb.center
                    + obb.axis[0] * (obb.halfExtents.x * float(x))
                    + obb.axis[1] * (obb.halfExtents.y * float(y))
                    + obb.axis[2] * (obb.halfExtents.z * float(z));
            }
        }
    }
    return verts;
}

template <typename ObbT>
inline bool pointInsideObb(const Vec3& p, const ObbT& obb, float eps = 1e-4f) {
    const Vec3 d = p - obb.center;
    for (int i = 0; i < 3; ++i) {
        const float dist = dot(d, obb.axis[i]);
        const float ext = getComp(obb.halfExtents, i);
        if (dist > ext + eps || dist < -ext - eps) return false;
    }
    return true;
}

inline float distanceSq(const Vec3& a, const Vec3& b) {
    const Vec3 d = a - b;
    return dot(d, d);
}
}

PhysicsWorld::PhysicsWorld() = default;
PhysicsWorld::~PhysicsWorld() = default;

void PhysicsWorld::setSolverIterations(int velocityIterations, int positionIterations) {
    m_solverIterations = std::max(1, velocityIterations);
    m_positionIterations = std::max(1, positionIterations);
}

void PhysicsWorld::setMaxSubSteps(int maxSubSteps) {
    m_maxSubSteps = std::clamp(maxSubSteps, 1, 16);
}

void PhysicsWorld::addBody(RigidBody* b) {
    if (!b) return;
    if (std::find(m_bodies.begin(), m_bodies.end(), b) == m_bodies.end()) {
        m_bodies.push_back(b);
    }
}

void PhysicsWorld::removeBody(RigidBody* b) {
    m_bodies.erase(std::remove(m_bodies.begin(), m_bodies.end(), b), m_bodies.end());
}

int PhysicsWorld::addDistanceJoint(const DistanceJointDesc& desc) {
    if (!desc.bodyA || !desc.bodyB || desc.bodyA == desc.bodyB) {
        return -1;
    }

    DistanceJointConstraint joint;
    joint.id = m_nextJointId++;
    joint.a = desc.bodyA;
    joint.b = desc.bodyB;
    joint.localAnchorA = desc.localAnchorA;
    joint.localAnchorB = desc.localAnchorB;
    joint.stiffness = std::clamp(desc.stiffness, 0.0f, 1.0f);
    joint.damping = std::max(0.0f, desc.damping);

    const Vec3 worldA = desc.bodyA->position + desc.localAnchorA;
    const Vec3 worldB = desc.bodyB->position + desc.localAnchorB;
    const float currentLength = std::sqrt(lengthSq(worldB - worldA));
    joint.restLength = desc.restLength >= 0.0f ? desc.restLength : currentLength;

    m_joints.push_back(joint);
    return joint.id;
}

void PhysicsWorld::removeDistanceJoint(int jointId) {
    m_joints.erase(std::remove_if(m_joints.begin(), m_joints.end(),
        [&](const DistanceJointConstraint& j) { return j.id == jointId; }), m_joints.end());
}

void PhysicsWorld::clearDistanceJoints() {
    m_joints.clear();
}

void PhysicsWorld::step(float dt) {
    if (dt <= 0.0f) return;

    dt = std::min(dt, m_maxFrameStep);
    m_accumulator += dt;

    int fixedSteps = 0;
    const int maxFixedStepsPerFrame = 8;
    while (m_accumulator >= FIXED_STEP && fixedSteps < maxFixedStepsPerFrame) {
        simulate(FIXED_STEP);
        m_accumulator -= FIXED_STEP;
        ++fixedSteps;
    }

    if (fixedSteps == maxFixedStepsPerFrame) {
        m_accumulator = 0.0f;
    }
}

void PhysicsWorld::simulate(float dt) {
    const int subSteps = computeAdaptiveSubSteps(dt);
    const float subDt = dt / static_cast<float>(subSteps);

    for (int sub = 0; sub < subSteps; ++sub) {
        ++m_frameCounter;

        std::unordered_map<RigidBody*, Vec3> startPositions;
        std::unordered_map<RigidBody*, Quat> startOrientations;
        startPositions.reserve(m_bodies.size());
        startOrientations.reserve(m_bodies.size());
        for (auto* b : m_bodies) {
            if (!b) continue;
            startPositions[b] = b->position;
            startOrientations[b] = b->orientation;
        }

        integrate(subDt);
        updateBroadphaseTree(subDt);

        auto pairs = buildBroadphasePairs();
        applyToiCcd(pairs, startPositions, startOrientations, subDt);

        updateBroadphaseTree(subDt);
        pairs = buildBroadphasePairs();

        std::vector<ContactManifold> manifolds;
        manifolds.reserve(pairs.size());
        for (const auto& pair : pairs) {
            ContactManifold m;
            if (buildManifold(pair.first, pair.second, m)) {
                manifolds.push_back(std::move(m));
            }
        }

        prepareManifolds(manifolds, subDt);
        warmStartManifolds(manifolds);
        prepareJointConstraints(subDt);
        warmStartJoints();

        for (int i = 0; i < m_solverIterations; ++i) {
            solveJointVelocityConstraints();
            solveVelocityConstraints(manifolds);
        }

        for (int i = 0; i < m_positionIterations; ++i) {
            solveJointPositionConstraints();
            solvePositionConstraints(manifolds);
        }

        cacheManifolds(manifolds);
        pruneManifoldCache();
        updateSleepStates(subDt);
    }
}

int PhysicsWorld::computeAdaptiveSubSteps(float dt) const {
    float maxMotion = 0.0f;
    float minHalfExtent = std::numeric_limits<float>::max();

    for (auto* b : m_bodies) {
        if (!b || b->type != BodyType::Dynamic) continue;

        const float sizeX = absf((b->localBoundsMax.x - b->localBoundsMin.x) * b->scale.x);
        const float sizeY = absf((b->localBoundsMax.y - b->localBoundsMin.y) * b->scale.y);
        const float sizeZ = absf((b->localBoundsMax.z - b->localBoundsMin.z) * b->scale.z);
        const float minExtent = std::max(0.001f, min3(sizeX, sizeY, sizeZ));
        minHalfExtent = std::min(minHalfExtent, minExtent * 0.5f);

        const float radius = 0.5f * std::sqrt(sizeX * sizeX + sizeY * sizeY + sizeZ * sizeZ);
        const float linearSpeed = std::sqrt(lengthSq(b->velocity));
        const float angularSpeed = std::sqrt(lengthSq(b->angularVelocity));
        const float estimatedMotion = (linearSpeed + angularSpeed * radius) * dt;
        maxMotion = std::max(maxMotion, estimatedMotion);
    }

    if (minHalfExtent == std::numeric_limits<float>::max()) {
        return 1;
    }

    const float safeMotion = std::max(0.001f, minHalfExtent * 0.35f);
    const int recommended = static_cast<int>(std::ceil(maxMotion / safeMotion));
    return std::clamp(recommended, 1, m_maxSubSteps);
}

void PhysicsWorld::integrate(float dt) {
    for (auto* b : m_bodies) {
        if (!b) continue;
        if (b->type != BodyType::Dynamic) continue;

        if (!b->isAwake) {
            if (lengthSq(b->force) > 1e-6f || lengthSq(b->torque) > 1e-6f) {
                b->wakeUp();
            } else {
                continue;
            }
        }

        Vec3 accel = m_gravity;
        if (b->invMass > 0.0f) {
            accel += b->force * b->invMass;
        }
        const Vec3 angularAccel = mulInvInertia(*b, b->torque);

        b->velocity += accel * dt;
        b->angularVelocity += angularAccel * dt;

        const float linearDamping = std::max(0.0f, 1.0f - b->linearDamping * dt);
        const float angularDamping = std::max(0.0f, 1.0f - b->angularDamping * dt);
        b->velocity = b->velocity * linearDamping;
        b->angularVelocity = b->angularVelocity * angularDamping;

        b->position += b->velocity * dt;
        b->orientation = integrateOrientation(b->orientation, b->angularVelocity, dt);

        b->force = {0,0,0};
        b->torque = {0,0,0};
    }
}

PhysicsWorld::OBB PhysicsWorld::computeObb(const RigidBody& body, const Vec3& atPosition, const Quat& atOrientation) const {
    OBB obb;
    const Quat q = normalize(atOrientation);

    const Vec3 scaledMin = {
        body.localBoundsMin.x * body.scale.x,
        body.localBoundsMin.y * body.scale.y,
        body.localBoundsMin.z * body.scale.z
    };
    const Vec3 scaledMax = {
        body.localBoundsMax.x * body.scale.x,
        body.localBoundsMax.y * body.scale.y,
        body.localBoundsMax.z * body.scale.z
    };

    const Vec3 localCenter = {
        0.5f * (scaledMin.x + scaledMax.x),
        0.5f * (scaledMin.y + scaledMax.y),
        0.5f * (scaledMin.z + scaledMax.z)
    };

    obb.halfExtents = {
        0.5f * absf(scaledMax.x - scaledMin.x),
        0.5f * absf(scaledMax.y - scaledMin.y),
        0.5f * absf(scaledMax.z - scaledMin.z)
    };

    obb.axis[0] = safeNormalize(q.rotate({1,0,0}));
    obb.axis[1] = safeNormalize(q.rotate({0,1,0}));
    obb.axis[2] = safeNormalize(q.rotate({0,0,1}));

    obb.center = atPosition + q.rotate(localCenter);
    return obb;
}

PhysicsWorld::BoundingSphere PhysicsWorld::computeBoundingSphere(const RigidBody& body, const Vec3& atPosition, const Quat& atOrientation, float motionPadding) const {
    const OBB obb = computeObb(body, atPosition, atOrientation);
    BoundingSphere sphere;
    sphere.center = obb.center;
    sphere.radius = std::sqrt(lengthSq(obb.halfExtents)) + std::max(0.0f, motionPadding);
    return sphere;
}

PhysicsWorld::AABB PhysicsWorld::mergeAabb(const AABB& a, const AABB& b) {
    return {
        {std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y), std::min(a.min.z, b.min.z)},
        {std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y), std::max(a.max.z, b.max.z)}
    };
}

bool PhysicsWorld::aabbOverlap(const AABB& a, const AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool PhysicsWorld::aabbContains(const AABB& a, const AABB& b) {
    return a.min.x <= b.min.x && a.min.y <= b.min.y && a.min.z <= b.min.z &&
           a.max.x >= b.max.x && a.max.y >= b.max.y && a.max.z >= b.max.z;
}

float PhysicsWorld::aabbPerimeter(const AABB& a) {
    const Vec3 e{a.max.x - a.min.x, a.max.y - a.min.y, a.max.z - a.min.z};
    return 2.0f * (e.x * e.y + e.y * e.z + e.z * e.x);
}

PhysicsWorld::AABB PhysicsWorld::computeFatAabb(const RigidBody& body, float dt) const {
    const OBB obb = computeObb(body, body.position, body.orientation);
    const float hx = obb.halfExtents.x;
    const float hy = obb.halfExtents.y;
    const float hz = obb.halfExtents.z;

    const Vec3 e{
        absf(obb.axis[0].x) * hx + absf(obb.axis[1].x) * hy + absf(obb.axis[2].x) * hz,
        absf(obb.axis[0].y) * hx + absf(obb.axis[1].y) * hy + absf(obb.axis[2].y) * hz,
        absf(obb.axis[0].z) * hx + absf(obb.axis[1].z) * hy + absf(obb.axis[2].z) * hz
    };

    const Vec3 linearPad{
        absf(body.velocity.x) * dt,
        absf(body.velocity.y) * dt,
        absf(body.velocity.z) * dt
    };
    const float radius = std::sqrt(lengthSq(obb.halfExtents));
    const float angularPadScalar = std::sqrt(lengthSq(body.angularVelocity)) * radius * dt;
    const Vec3 pad{
        0.05f + linearPad.x + angularPadScalar,
        0.05f + linearPad.y + angularPadScalar,
        0.05f + linearPad.z + angularPadScalar
    };

    return {obb.center - e - pad, obb.center + e + pad};
}

int PhysicsWorld::allocateTreeNode() {
    if (!m_freeTreeNodes.empty()) {
        const int idx = m_freeTreeNodes.back();
        m_freeTreeNodes.pop_back();
        m_treeNodes[idx] = DynamicTreeNode{};
        return idx;
    }
    m_treeNodes.push_back(DynamicTreeNode{});
    return static_cast<int>(m_treeNodes.size()) - 1;
}

void PhysicsWorld::freeTreeNode(int node) {
    if (node < 0 || node >= static_cast<int>(m_treeNodes.size())) return;
    m_treeNodes[node] = DynamicTreeNode{};
    m_freeTreeNodes.push_back(node);
}

int PhysicsWorld::createLeaf(RigidBody* body, const AABB& fatBox) {
    const int leaf = allocateTreeNode();
    auto& n = m_treeNodes[leaf];
    n.box = fatBox;
    n.body = body;
    n.height = 0;
    n.left = -1;
    n.right = -1;
    n.parent = -1;
    return leaf;
}

void PhysicsWorld::destroyLeaf(int leaf) {
    freeTreeNode(leaf);
}

void PhysicsWorld::refitUpward(int node) {
    while (node != -1) {
        auto& n = m_treeNodes[node];
        if (!n.isLeaf()) {
            const int l = n.left;
            const int r = n.right;
            n.box = mergeAabb(m_treeNodes[l].box, m_treeNodes[r].box);
            n.height = 1 + std::max(m_treeNodes[l].height, m_treeNodes[r].height);
        }
        node = n.parent;
    }
}

int PhysicsWorld::selectBestSibling(const AABB& box) const {
    int idx = m_treeRoot;
    while (idx != -1 && !m_treeNodes[idx].isLeaf()) {
        const int l = m_treeNodes[idx].left;
        const int r = m_treeNodes[idx].right;

        const float costL = aabbPerimeter(mergeAabb(m_treeNodes[l].box, box)) - aabbPerimeter(m_treeNodes[l].box);
        const float costR = aabbPerimeter(mergeAabb(m_treeNodes[r].box, box)) - aabbPerimeter(m_treeNodes[r].box);
        idx = costL < costR ? l : r;
    }
    return idx;
}

void PhysicsWorld::insertLeaf(int leaf) {
    if (m_treeRoot == -1) {
        m_treeRoot = leaf;
        m_treeNodes[leaf].parent = -1;
        return;
    }

    const int sibling = selectBestSibling(m_treeNodes[leaf].box);
    const int oldParent = m_treeNodes[sibling].parent;

    const int newParent = allocateTreeNode();
    auto& p = m_treeNodes[newParent];
    p.parent = oldParent;
    p.body = nullptr;
    p.left = sibling;
    p.right = leaf;
    p.height = m_treeNodes[sibling].height + 1;
    p.box = mergeAabb(m_treeNodes[sibling].box, m_treeNodes[leaf].box);

    m_treeNodes[sibling].parent = newParent;
    m_treeNodes[leaf].parent = newParent;

    if (oldParent == -1) {
        m_treeRoot = newParent;
    } else {
        if (m_treeNodes[oldParent].left == sibling) m_treeNodes[oldParent].left = newParent;
        else m_treeNodes[oldParent].right = newParent;
    }

    refitUpward(newParent);
}

void PhysicsWorld::removeLeaf(int leaf) {
    if (leaf == m_treeRoot) {
        m_treeRoot = -1;
        return;
    }

    const int parent = m_treeNodes[leaf].parent;
    const int grand = m_treeNodes[parent].parent;
    const int sibling = (m_treeNodes[parent].left == leaf) ? m_treeNodes[parent].right : m_treeNodes[parent].left;

    if (grand != -1) {
        if (m_treeNodes[grand].left == parent) m_treeNodes[grand].left = sibling;
        else m_treeNodes[grand].right = sibling;
        m_treeNodes[sibling].parent = grand;
        freeTreeNode(parent);
        refitUpward(grand);
    } else {
        m_treeRoot = sibling;
        m_treeNodes[sibling].parent = -1;
        freeTreeNode(parent);
    }

    m_treeNodes[leaf].parent = -1;
}

void PhysicsWorld::updateBroadphaseTree(float dt) {
    std::unordered_set<RigidBody*> active;
    active.reserve(m_bodies.size());
    for (auto* b : m_bodies) if (b) active.insert(b);

    for (auto it = m_bodyLeaf.begin(); it != m_bodyLeaf.end(); ) {
        if (active.find(it->first) == active.end()) {
            const int leaf = it->second;
            removeLeaf(leaf);
            destroyLeaf(leaf);
            it = m_bodyLeaf.erase(it);
        } else {
            ++it;
        }
    }

    for (auto* b : m_bodies) {
        if (!b) continue;
        const AABB fat = computeFatAabb(*b, dt);

        auto it = m_bodyLeaf.find(b);
        if (it == m_bodyLeaf.end()) {
            const int leaf = createLeaf(b, fat);
            m_bodyLeaf[b] = leaf;
            insertLeaf(leaf);
            continue;
        }

        const int leaf = it->second;
        if (aabbContains(m_treeNodes[leaf].box, fat)) continue;

        removeLeaf(leaf);
        m_treeNodes[leaf].box = fat;
        insertLeaf(leaf);
    }
}

std::vector<std::pair<RigidBody*, RigidBody*>> PhysicsWorld::buildBroadphasePairs() const {
    std::vector<std::pair<RigidBody*, RigidBody*>> pairs;
    if (m_treeRoot == -1) return pairs;

    std::unordered_set<uint64_t> seen;
    std::vector<int> stack;
    stack.reserve(64);

    for (const auto& [body, leaf] : m_bodyLeaf) {
        if (!body) continue;
        if (leaf < 0 || leaf >= static_cast<int>(m_treeNodes.size())) continue;

        const AABB query = m_treeNodes[leaf].box;
        stack.clear();
        stack.push_back(m_treeRoot);

        while (!stack.empty()) {
            const int nodeIdx = stack.back();
            stack.pop_back();
            if (nodeIdx < 0 || nodeIdx == leaf) continue;

            const auto& node = m_treeNodes[nodeIdx];
            if (!aabbOverlap(query, node.box)) continue;

            if (node.isLeaf()) {
                RigidBody* other = node.body;
                if (!other || other == body) continue;
                if (body->invMass + other->invMass <= 0.0f) continue;

                const uint64_t key = makePairKey(body, other);
                if (seen.insert(key).second) {
                    pairs.emplace_back(body, other);
                }
            } else {
                stack.push_back(node.left);
                stack.push_back(node.right);
            }
        }
    }

    return pairs;
}

bool PhysicsWorld::computeSweptSphereToi(
    const RigidBody& a,
    const Vec3& startPosA,
    const Quat& startOrientA,
    const Vec3& velA,
    const RigidBody& b,
    const Vec3& startPosB,
    const Quat& startOrientB,
    const Vec3& velB,
    float dt,
    float& outToi,
    Vec3& outNormal
) const {
    if (dt <= 0.0f) return false;

    const BoundingSphere sa = computeBoundingSphere(a, startPosA, startOrientA, 0.0f);
    const BoundingSphere sb = computeBoundingSphere(b, startPosB, startOrientB, 0.0f);

    const Vec3 relStart = sb.center - sa.center;
    const Vec3 relDisp = (velB - velA) * dt;
    const float radius = sa.radius + sb.radius;

    const float c = dot(relStart, relStart) - radius * radius;
    if (c <= 0.0f) return false;

    const float aCoef = dot(relDisp, relDisp);
    if (aCoef <= 1e-10f) return false;

    const float bCoef = 2.0f * dot(relStart, relDisp);
    const float disc = bCoef * bCoef - 4.0f * aCoef * c;
    if (disc < 0.0f) return false;

    const float sqrtDisc = std::sqrt(disc);
    const float u = (-bCoef - sqrtDisc) / (2.0f * aCoef);
    if (u < 0.0f || u > 1.0f) return false;

    outToi = u;
    outNormal = safeNormalize(relStart + relDisp * u);
    if (lengthSq(outNormal) <= 1e-8f) outNormal = safeNormalize(relStart);
    return lengthSq(outNormal) > 1e-8f;
}

void PhysicsWorld::applyToiCcd(
    const std::vector<std::pair<RigidBody*, RigidBody*>>& pairs,
    const std::unordered_map<RigidBody*, Vec3>& startPositions,
    const std::unordered_map<RigidBody*, Quat>& startOrientations,
    float dt
) {
    if (dt <= 0.0f) return;

    std::unordered_map<RigidBody*, float> toiScale;
    std::unordered_map<RigidBody*, Vec3> collisionNormals;
    toiScale.reserve(m_bodies.size());
    collisionNormals.reserve(m_bodies.size());

    for (const auto& pair : pairs) {
        RigidBody* a = pair.first;
        RigidBody* b = pair.second;
        if (!a || !b) continue;

        const bool ccdA = (a->type == BodyType::Dynamic && a->enableCCD);
        const bool ccdB = (b->type == BodyType::Dynamic && b->enableCCD);
        if (!ccdA && !ccdB) continue;

        auto itPosA = startPositions.find(a);
        auto itPosB = startPositions.find(b);
        auto itOriA = startOrientations.find(a);
        auto itOriB = startOrientations.find(b);
        if (itPosA == startPositions.end() || itPosB == startPositions.end()) continue;
        if (itOriA == startOrientations.end() || itOriB == startOrientations.end()) continue;

        float toi = 1.0f;
        Vec3 normal{0,0,0};
        if (!computeSweptSphereToi(
            *a, itPosA->second, itOriA->second, a->velocity,
            *b, itPosB->second, itOriB->second, b->velocity,
            dt, toi, normal)) {
            continue;
        }
        if (toi >= 1.0f) continue;

        if (ccdA) {
            auto [it, inserted] = toiScale.emplace(a, toi);
            if (!inserted) it->second = std::min(it->second, toi);
            collisionNormals[a] -= normal;
        }
        if (ccdB) {
            auto [it, inserted] = toiScale.emplace(b, toi);
            if (!inserted) it->second = std::min(it->second, toi);
            collisionNormals[b] += normal;
        }
    }

    for (auto& [body, scale] : toiScale) {
        if (!body) continue;
        auto itStart = startPositions.find(body);
        auto itStartOri = startOrientations.find(body);
        if (itStart == startPositions.end() || itStartOri == startOrientations.end()) continue;

        scale = clamp01(scale);
        if (scale >= 1.0f) continue;

        body->position = itStart->second + body->velocity * (dt * scale);
        body->orientation = integrateOrientation(itStartOri->second, body->angularVelocity, dt * scale);

        Vec3 n = safeNormalize(collisionNormals[body]);
        if (lengthSq(n) > 1e-8f) {
            const float vn = dot(body->velocity, n);
            if (vn < 0.0f) {
                body->velocity -= n * vn;
            }
        }
        body->wakeUp();
    }
}

uint64_t PhysicsWorld::makePairKey(const RigidBody* a, const RigidBody* b) const {
    const uintptr_t pa = reinterpret_cast<uintptr_t>(a);
    const uintptr_t pb = reinterpret_cast<uintptr_t>(b);
    const uintptr_t low = std::min(pa, pb);
    const uintptr_t high = std::max(pa, pb);
    uint64_t key = static_cast<uint64_t>(low);
    key ^= static_cast<uint64_t>(high) + 0x9e3779b97f4a7c15ULL + (key << 6) + (key >> 2);
    return key;
}

bool PhysicsWorld::buildManifold(RigidBody* a, RigidBody* b, ContactManifold& outManifold) const {
    if (!a || !b) return false;

    const OBB obbA = computeObb(*a, a->position, a->orientation);
    const OBB obbB = computeObb(*b, b->position, b->orientation);

    const Vec3 tWorld = obbB.center - obbA.center;
    float t[3] = {
        dot(tWorld, obbA.axis[0]),
        dot(tWorld, obbA.axis[1]),
        dot(tWorld, obbA.axis[2])
    };

    float R[3][3];
    float AbsR[3][3];
    constexpr float EPS = 1e-5f;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            R[i][j] = dot(obbA.axis[i], obbB.axis[j]);
            AbsR[i][j] = absf(R[i][j]) + EPS;
        }
    }

    const float aHalf[3] = { obbA.halfExtents.x, obbA.halfExtents.y, obbA.halfExtents.z };
    const float bHalf[3] = { obbB.halfExtents.x, obbB.halfExtents.y, obbB.halfExtents.z };

    float minOverlap = std::numeric_limits<float>::max();
    Vec3 bestNormal{0,1,0};
    bool hasAxis = false;

    auto testAxis = [&](float dist, float ra, float rb, const Vec3& axis, float axisSign) -> bool {
        const float overlap = ra + rb - dist;
        if (overlap < 0.0f) return false;
        if (lengthSq(axis) <= 1e-10f) return true;

        Vec3 n = safeNormalize(axis);
        if (axisSign < 0.0f) n = n * -1.0f;
        if (overlap < minOverlap) {
            minOverlap = overlap;
            bestNormal = n;
            hasAxis = true;
        }
        return true;
    };

    for (int i = 0; i < 3; ++i) {
        const float ra = aHalf[i];
        const float rb = bHalf[0] * AbsR[i][0] + bHalf[1] * AbsR[i][1] + bHalf[2] * AbsR[i][2];
        const float dist = absf(t[i]);
        const float sign = t[i] >= 0.0f ? 1.0f : -1.0f;
        if (!testAxis(dist, ra, rb, obbA.axis[i], sign)) return false;
    }

    for (int j = 0; j < 3; ++j) {
        const float tB = t[0] * R[0][j] + t[1] * R[1][j] + t[2] * R[2][j];
        const float ra = aHalf[0] * AbsR[0][j] + aHalf[1] * AbsR[1][j] + aHalf[2] * AbsR[2][j];
        const float rb = bHalf[j];
        const float dist = absf(tB);
        const float sign = tB >= 0.0f ? 1.0f : -1.0f;
        if (!testAxis(dist, ra, rb, obbB.axis[j], sign)) return false;
    }

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            const Vec3 axis = cross(obbA.axis[i], obbB.axis[j]);
            const float axisLenSq = lengthSq(axis);
            if (axisLenSq <= 1e-10f) continue;

            const int i1 = (i + 1) % 3;
            const int i2 = (i + 2) % 3;
            const int j1 = (j + 1) % 3;
            const int j2 = (j + 2) % 3;

            const float ra = aHalf[i1] * AbsR[i2][j] + aHalf[i2] * AbsR[i1][j];
            const float rb = bHalf[j1] * AbsR[i][j2] + bHalf[j2] * AbsR[i][j1];
            const float dist = absf(t[i2] * R[i1][j] - t[i1] * R[i2][j]);
            const float sign = dot(axis, tWorld) >= 0.0f ? 1.0f : -1.0f;
            if (!testAxis(dist, ra, rb, axis, sign)) return false;
        }
    }

    if (!hasAxis) {
        bestNormal = safeNormalize(tWorld);
        if (lengthSq(bestNormal) <= 1e-8f) bestNormal = {0,1,0};
        minOverlap = 0.0f;
    }

    std::vector<Vec3> candidates;
    candidates.reserve(16);

    const auto vertsA = computeObbVertices(obbA);
    const auto vertsB = computeObbVertices(obbB);
    for (const auto& v : vertsA) {
        if (pointInsideObb(v, obbB)) candidates.push_back(v);
    }
    for (const auto& v : vertsB) {
        if (pointInsideObb(v, obbA)) candidates.push_back(v);
    }

    auto support = [](const OBB& box, const Vec3& dir) -> Vec3 {
        Vec3 p = box.center;
        const float d0 = dot(dir, box.axis[0]);
        const float d1 = dot(dir, box.axis[1]);
        const float d2 = dot(dir, box.axis[2]);
        p += box.axis[0] * (d0 >= 0.0f ? box.halfExtents.x : -box.halfExtents.x);
        p += box.axis[1] * (d1 >= 0.0f ? box.halfExtents.y : -box.halfExtents.y);
        p += box.axis[2] * (d2 >= 0.0f ? box.halfExtents.z : -box.halfExtents.z);
        return p;
    };
    candidates.push_back((support(obbA, bestNormal) + support(obbB, bestNormal * -1.0f)) * 0.5f);

    std::vector<Vec3> unique;
    unique.reserve(candidates.size());
    for (const auto& c : candidates) {
        bool isDuplicate = false;
        for (const auto& u : unique) {
            if (distanceSq(c, u) < 1e-5f) {
                isDuplicate = true;
                break;
            }
        }
        if (!isDuplicate) unique.push_back(c);
    }

    if (unique.empty()) return false;

    std::vector<Vec3> selected;
    if (unique.size() <= 4) {
        selected = unique;
    } else {
        Vec3 centroid{0,0,0};
        for (const auto& p : unique) centroid += p;
        centroid = centroid * (1.0f / float(unique.size()));

        const Vec3 t1 = pickPerpendicular(bestNormal);
        const Vec3 t2 = safeNormalize(cross(bestNormal, t1));

        auto pickExtreme = [&](const Vec3& axis, bool maxDir) {
            float best = maxDir ? -std::numeric_limits<float>::max() : std::numeric_limits<float>::max();
            int bestIdx = -1;
            for (int i = 0; i < static_cast<int>(unique.size()); ++i) {
                const float p = dot(unique[i] - centroid, axis);
                if ((maxDir && p > best) || (!maxDir && p < best)) {
                    best = p;
                    bestIdx = i;
                }
            }
            if (bestIdx >= 0) {
                const Vec3 candidate = unique[bestIdx];
                bool dup = false;
                for (const auto& s : selected) {
                    if (distanceSq(s, candidate) < 1e-5f) {
                        dup = true;
                        break;
                    }
                }
                if (!dup) selected.push_back(candidate);
            }
        };

        pickExtreme(t1, true);
        pickExtreme(t1, false);
        pickExtreme(t2, true);
        pickExtreme(t2, false);

        while (selected.size() < 4) {
            float bestD = -1.0f;
            int bestIdx = -1;
            for (int i = 0; i < static_cast<int>(unique.size()); ++i) {
                const Vec3 candidate = unique[i];
                bool dup = false;
                for (const auto& s : selected) {
                    if (distanceSq(s, candidate) < 1e-5f) {
                        dup = true;
                        break;
                    }
                }
                if (dup) continue;

                float minD = std::numeric_limits<float>::max();
                for (const auto& s : selected) {
                    minD = std::min(minD, distanceSq(candidate, s));
                }
                if (selected.empty()) minD = distanceSq(candidate, centroid);

                if (minD > bestD) {
                    bestD = minD;
                    bestIdx = i;
                }
            }
            if (bestIdx < 0) break;
            selected.push_back(unique[bestIdx]);
        }
    }

    outManifold = {};
    outManifold.a = a;
    outManifold.b = b;
    outManifold.key = makePairKey(a, b);
    outManifold.normal = bestNormal;
    outManifold.restitution = std::min(a->restitution, b->restitution);
    outManifold.staticFriction = std::sqrt(std::max(0.0f, a->staticFriction) * std::max(0.0f, b->staticFriction));
    outManifold.dynamicFriction = std::sqrt(std::max(0.0f, a->dynamicFriction) * std::max(0.0f, b->dynamicFriction));
    outManifold.points.reserve(std::min<size_t>(4, selected.size()));

    for (const auto& p : selected) {
        ManifoldPoint mp;
        mp.point = p;
        mp.penetration = std::max(0.0f, minOverlap);
        outManifold.points.push_back(mp);
        if (outManifold.points.size() == 4) break;
    }

    if (outManifold.points.empty()) return false;

    const Vec3 relativeVelocity = b->velocity - a->velocity;
    if (std::fabs(dot(relativeVelocity, outManifold.normal)) < m_restitutionThreshold) {
        outManifold.restitution = 0.0f;
    }

    return true;
}

void PhysicsWorld::prepareManifolds(std::vector<ContactManifold>& manifolds, float dt) {
    if (dt <= 0.0f) return;

    for (auto& m : manifolds) {
        if (!m.a || !m.b) continue;

        ManifoldCacheEntry* cache = nullptr;
        auto it = m_manifoldCache.find(m.key);
        if (it != m_manifoldCache.end() && dot(it->second.normal, m.normal) > 0.6f) {
            cache = &it->second;
        }

        for (auto& p : m.points) {
            p.rA = p.point - m.a->position;
            p.rB = p.point - m.b->position;

            if (cache) {
                float bestDist = std::numeric_limits<float>::max();
                int bestIdx = -1;
                for (int i = 0; i < cache->pointCount; ++i) {
                    const float d = distanceSq(cache->points[i].point, p.point);
                    if (d < bestDist) {
                        bestDist = d;
                        bestIdx = i;
                    }
                }
                if (bestIdx >= 0 && bestDist < 0.04f * 0.04f) {
                    p.normalImpulse = cache->points[bestIdx].normalImpulse * m_warmStartScale;
                    p.tangentImpulse = cache->points[bestIdx].tangentImpulse * m_warmStartScale;
                }
            }

            Vec3 rv = (m.b->velocity + cross(m.b->angularVelocity, p.rB))
                    - (m.a->velocity + cross(m.a->angularVelocity, p.rA));
            Vec3 tangent = rv - m.normal * dot(rv, m.normal);
            if (lengthSq(tangent) <= 1e-8f) tangent = pickPerpendicular(m.normal);
            else tangent = safeNormalize(tangent);
            p.tangent = tangent;

            const float invMassA = m.a->invMass;
            const float invMassB = m.b->invMass;

            const Vec3 rnA = cross(p.rA, m.normal);
            const Vec3 rnB = cross(p.rB, m.normal);
            const float angA = dot(cross(mulInvInertia(*m.a, rnA), p.rA), m.normal);
            const float angB = dot(cross(mulInvInertia(*m.b, rnB), p.rB), m.normal);
            const float normalDenom = invMassA + invMassB + angA + angB;
            p.normalMass = normalDenom > 1e-8f ? 1.0f / normalDenom : 0.0f;

            const Vec3 rtA = cross(p.rA, p.tangent);
            const Vec3 rtB = cross(p.rB, p.tangent);
            const float tangAngA = dot(cross(mulInvInertia(*m.a, rtA), p.rA), p.tangent);
            const float tangAngB = dot(cross(mulInvInertia(*m.b, rtB), p.rB), p.tangent);
            const float tangentDenom = invMassA + invMassB + tangAngA + tangAngB;
            p.tangentMass = tangentDenom > 1e-8f ? 1.0f / tangentDenom : 0.0f;

            p.bias = (m_baumgarte / dt) * std::max(p.penetration - m_penetrationSlop, 0.0f);
        }
    }
}

void PhysicsWorld::warmStartManifolds(std::vector<ContactManifold>& manifolds) {
    for (auto& m : manifolds) {
        if (!m.a || !m.b) continue;

        for (auto& p : m.points) {
            const Vec3 impulse = m.normal * p.normalImpulse + p.tangent * p.tangentImpulse;
            if (lengthSq(impulse) <= 1e-12f) continue;

            if (m.a->type == BodyType::Dynamic) {
                m.a->velocity -= impulse * m.a->invMass;
                m.a->angularVelocity -= mulInvInertia(*m.a, cross(p.rA, impulse));
                m.a->wakeUp();
            }
            if (m.b->type == BodyType::Dynamic) {
                m.b->velocity += impulse * m.b->invMass;
                m.b->angularVelocity += mulInvInertia(*m.b, cross(p.rB, impulse));
                m.b->wakeUp();
            }
        }
    }
}

void PhysicsWorld::solveVelocityConstraints(std::vector<ContactManifold>& manifolds) {
    for (auto& m : manifolds) {
        if (!m.a || !m.b) continue;
        if (m.a->invMass + m.b->invMass <= 0.0f) continue;

        for (auto& p : m.points) {
            Vec3 rv = (m.b->velocity + cross(m.b->angularVelocity, p.rB))
                    - (m.a->velocity + cross(m.a->angularVelocity, p.rA));

            const float vn = dot(rv, m.normal);
            float lambdaN = p.normalMass * (-(vn) + p.bias);
            const float oldN = p.normalImpulse;
            p.normalImpulse = std::max(oldN + lambdaN, 0.0f);
            lambdaN = p.normalImpulse - oldN;

            const Vec3 impulseN = m.normal * lambdaN;
            if (m.a->type == BodyType::Dynamic) {
                m.a->velocity -= impulseN * m.a->invMass;
                m.a->angularVelocity -= mulInvInertia(*m.a, cross(p.rA, impulseN));
            }
            if (m.b->type == BodyType::Dynamic) {
                m.b->velocity += impulseN * m.b->invMass;
                m.b->angularVelocity += mulInvInertia(*m.b, cross(p.rB, impulseN));
            }

            rv = (m.b->velocity + cross(m.b->angularVelocity, p.rB))
               - (m.a->velocity + cross(m.a->angularVelocity, p.rA));

            const float vt = dot(rv, p.tangent);
            float lambdaT = p.tangentMass * (-vt);
            const float oldT = p.tangentImpulse;
            float newT = oldT + lambdaT;

            const float maxStatic = m.staticFriction * p.normalImpulse;
            const float maxDynamic = m.dynamicFriction * p.normalImpulse;
            if (std::fabs(newT) > maxStatic) {
                newT = std::clamp(newT, -maxDynamic, maxDynamic);
            } else {
                newT = std::clamp(newT, -maxStatic, maxStatic);
            }

            lambdaT = newT - oldT;
            p.tangentImpulse = newT;

            const Vec3 impulseT = p.tangent * lambdaT;
            if (m.a->type == BodyType::Dynamic) {
                m.a->velocity -= impulseT * m.a->invMass;
                m.a->angularVelocity -= mulInvInertia(*m.a, cross(p.rA, impulseT));
            }
            if (m.b->type == BodyType::Dynamic) {
                m.b->velocity += impulseT * m.b->invMass;
                m.b->angularVelocity += mulInvInertia(*m.b, cross(p.rB, impulseT));
            }
        }
    }
}

void PhysicsWorld::solvePositionConstraints(std::vector<ContactManifold>& manifolds) {
    for (auto& m : manifolds) {
        if (!m.a || !m.b) continue;
        if (m.a->invMass + m.b->invMass <= 0.0f) continue;

        ContactManifold fresh;
        if (!buildManifold(m.a, m.b, fresh)) continue;
        if (fresh.points.empty()) continue;

        const float invMassA = m.a->invMass;
        const float invMassB = m.b->invMass;
        const float invMassSum = invMassA + invMassB;

        const float scale = 1.0f / float(fresh.points.size());
        for (const auto& p : fresh.points) {
            const float correctionMagnitude = (std::max(p.penetration - m_penetrationSlop, 0.0f) / invMassSum) * m_positionCorrection * scale;
            if (correctionMagnitude <= 0.0f) continue;

            const Vec3 correction = fresh.normal * correctionMagnitude;
            if (m.a->type == BodyType::Dynamic) m.a->position -= correction * invMassA;
            if (m.b->type == BodyType::Dynamic) m.b->position += correction * invMassB;
        }
    }
}

void PhysicsWorld::cacheManifolds(const std::vector<ContactManifold>& manifolds) {
    for (const auto& m : manifolds) {
        if (!m.a || !m.b) continue;

        ManifoldCacheEntry& entry = m_manifoldCache[m.key];
        entry.normal = m.normal;
        entry.pointCount = std::min<int>(4, m.points.size());
        for (int i = 0; i < entry.pointCount; ++i) {
            entry.points[i].point = m.points[i].point;
            entry.points[i].normalImpulse = m.points[i].normalImpulse;
            entry.points[i].tangentImpulse = m.points[i].tangentImpulse;
        }
        entry.lastTouchedFrame = m_frameCounter;
    }
}

void PhysicsWorld::pruneManifoldCache() {
    for (auto it = m_manifoldCache.begin(); it != m_manifoldCache.end(); ) {
        const uint32_t age = m_frameCounter - it->second.lastTouchedFrame;
        if (age > static_cast<uint32_t>(m_cachePersistenceFrames)) {
            it = m_manifoldCache.erase(it);
        } else {
            ++it;
        }
    }
}

void PhysicsWorld::prepareJointConstraints(float dt) {
    if (dt <= 0.0f) return;

    for (auto& j : m_joints) {
        if (!j.a || !j.b) {
            j.effectiveMass = 0.0f;
            continue;
        }

        const Vec3 worldA = j.a->position + j.localAnchorA;
        const Vec3 worldB = j.b->position + j.localAnchorB;
        const Vec3 delta = worldB - worldA;
        const float lenSq = lengthSq(delta);
        const float len = lenSq > 1e-10f ? std::sqrt(lenSq) : 0.0f;

        j.axis = len > 1e-5f ? delta * (1.0f / len) : Vec3{1,0,0};
        j.rA = worldA - j.a->position;
        j.rB = worldB - j.b->position;

        const float invMassA = j.a->invMass;
        const float invMassB = j.b->invMass;
        const Vec3 raXn = cross(j.rA, j.axis);
        const Vec3 rbXn = cross(j.rB, j.axis);
        const float angA = dot(cross(mulInvInertia(*j.a, raXn), j.rA), j.axis);
        const float angB = dot(cross(mulInvInertia(*j.b, rbXn), j.rB), j.axis);
        const float denom = invMassA + invMassB + angA + angB;
        j.effectiveMass = denom > 1e-8f ? 1.0f / denom : 0.0f;

        const float error = len - j.restLength;
        j.bias = (j.stiffness / dt) * error;
    }
}

void PhysicsWorld::warmStartJoints() {
    for (auto& j : m_joints) {
        if (!j.a || !j.b) continue;
        if (std::fabs(j.accumulatedImpulse) <= 1e-8f) continue;

        const Vec3 impulse = j.axis * j.accumulatedImpulse;
        if (j.a->type == BodyType::Dynamic) {
            j.a->velocity -= impulse * j.a->invMass;
            j.a->angularVelocity -= mulInvInertia(*j.a, cross(j.rA, impulse));
            j.a->wakeUp();
        }
        if (j.b->type == BodyType::Dynamic) {
            j.b->velocity += impulse * j.b->invMass;
            j.b->angularVelocity += mulInvInertia(*j.b, cross(j.rB, impulse));
            j.b->wakeUp();
        }
    }
}

void PhysicsWorld::solveJointVelocityConstraints() {
    for (auto& j : m_joints) {
        if (!j.a || !j.b) continue;
        if (j.effectiveMass <= 0.0f) continue;

        const Vec3 velA = j.a->velocity + cross(j.a->angularVelocity, j.rA);
        const Vec3 velB = j.b->velocity + cross(j.b->angularVelocity, j.rB);
        const float rel = dot(velB - velA, j.axis);

        const float lambda = -j.effectiveMass * (rel * (1.0f + j.damping) + j.bias);
        j.accumulatedImpulse += lambda;

        const Vec3 impulse = j.axis * lambda;
        if (j.a->type == BodyType::Dynamic) {
            j.a->velocity -= impulse * j.a->invMass;
            j.a->angularVelocity -= mulInvInertia(*j.a, cross(j.rA, impulse));
        }
        if (j.b->type == BodyType::Dynamic) {
            j.b->velocity += impulse * j.b->invMass;
            j.b->angularVelocity += mulInvInertia(*j.b, cross(j.rB, impulse));
        }
    }
}

void PhysicsWorld::solveJointPositionConstraints() {
    for (auto& j : m_joints) {
        if (!j.a || !j.b) continue;

        const Vec3 worldA = j.a->position + j.localAnchorA;
        const Vec3 worldB = j.b->position + j.localAnchorB;
        const Vec3 delta = worldB - worldA;
        const float lenSq = lengthSq(delta);
        if (lenSq <= 1e-10f) continue;

        const float len = std::sqrt(lenSq);
        const Vec3 axis = delta * (1.0f / len);
        const float error = len - j.restLength;
        if (std::fabs(error) <= 1e-4f) continue;

        const float invMassA = j.a->invMass;
        const float invMassB = j.b->invMass;
        const float invMassSum = invMassA + invMassB;
        if (invMassSum <= 0.0f) continue;

        const Vec3 correction = axis * (error * j.stiffness * 0.5f);
        if (j.a->type == BodyType::Dynamic) {
            j.a->position += correction * (invMassA / invMassSum);
        }
        if (j.b->type == BodyType::Dynamic) {
            j.b->position -= correction * (invMassB / invMassSum);
        }
    }
}

void PhysicsWorld::updateSleepStates(float dt) {
    constexpr float sleepLinearSpeed = 0.04f;
    constexpr float sleepLinearSpeedSq = sleepLinearSpeed * sleepLinearSpeed;
    constexpr float sleepAngularSpeed = 0.06f;
    constexpr float sleepAngularSpeedSq = sleepAngularSpeed * sleepAngularSpeed;
    constexpr float sleepDelay = 0.6f;

    for (auto* b : m_bodies) {
        if (!b || b->type != BodyType::Dynamic) continue;
        if (!b->enableSleep) {
            b->wakeUp();
            continue;
        }

        const bool lowLinear = lengthSq(b->velocity) < sleepLinearSpeedSq;
        const bool lowAngular = lengthSq(b->angularVelocity) < sleepAngularSpeedSq;
        const bool lowForce = lengthSq(b->force) < 1e-6f;
        const bool lowTorque = lengthSq(b->torque) < 1e-6f;

        if (lowLinear && lowAngular && lowForce && lowTorque) {
            b->sleepTimer += dt;
            if (b->sleepTimer >= sleepDelay) {
                b->isAwake = false;
                b->velocity = {0,0,0};
                b->angularVelocity = {0,0,0};
            }
        } else {
            b->wakeUp();
        }
    }
}

bool PhysicsWorld::raycastObb(const Vec3& origin, const Vec3& direction, const OBB& obb, float maxDist, float& outT, Vec3& outNormal) {
    const Vec3 rel = origin - obb.center;
    const Vec3 oLocal = {
        dot(rel, obb.axis[0]),
        dot(rel, obb.axis[1]),
        dot(rel, obb.axis[2])
    };
    const Vec3 dLocal = {
        dot(direction, obb.axis[0]),
        dot(direction, obb.axis[1]),
        dot(direction, obb.axis[2])
    };

    float tMin = 0.0f;
    float tMax = maxDist;
    int hitAxis = -1;
    float hitSign = 1.0f;

    for (int i = 0; i < 3; ++i) {
        const float o = getComp(oLocal, i);
        const float d = getComp(dLocal, i);
        const float minB = -getComp(obb.halfExtents, i);
        const float maxB =  getComp(obb.halfExtents, i);

        if (std::fabs(d) < 1e-7f) {
            if (o < minB || o > maxB) return false;
            continue;
        }

        float t1 = (minB - o) / d;
        float t2 = (maxB - o) / d;
        float sign1 = -1.0f;
        float sign2 = 1.0f;
        if (t1 > t2) {
            std::swap(t1, t2);
            std::swap(sign1, sign2);
        }

        if (t1 > tMin) {
            tMin = t1;
            hitAxis = i;
            hitSign = sign1;
        }
        tMax = std::min(tMax, t2);
        if (tMin > tMax) return false;
    }

    if (tMin < 0.0f || tMin > maxDist) return false;

    outT = tMin;
    if (hitAxis >= 0) outNormal = obb.axis[hitAxis] * hitSign;
    else outNormal = safeNormalize(direction * -1.0f);
    return true;
}

Vec3 PhysicsWorld::mulInvInertia(const RigidBody& body, const Vec3& v) {
    const Quat q = normalize(body.orientation);
    const Vec3 local = q.conjugate().rotate(v);
    const Vec3 localApplied{
        body.invInertia.x * local.x,
        body.invInertia.y * local.y,
        body.invInertia.z * local.z
    };
    return q.rotate(localApplied);
}

RaycastHit PhysicsWorld::raycast(const Vec3& origin, const Vec3& direction, float maxDist) {
    RaycastHit best;
    if (maxDist <= 0.0f) return best;

    const Vec3 dir = safeNormalize(direction);
    if (lengthSq(dir) <= 1e-8f) return best;

    float closest = maxDist;
    for (auto* body : m_bodies) {
        if (!body) continue;

        const OBB obb = computeObb(*body, body->position, body->orientation);
        float t = 0.0f;
        Vec3 n{0,0,0};
        if (!raycastObb(origin, dir, obb, closest, t, n)) continue;

        closest = t;
        best.hit = true;
        best.distance = t;
        best.body = body;
        best.normal = n;
        best.point = origin + dir * t;
    }

    return best;
}

}
