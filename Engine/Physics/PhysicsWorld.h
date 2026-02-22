#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "../Core/RexMath.h"

namespace rex {

class RigidBody;

struct RaycastHit {
    bool hit = false;
    Vec3 point{0,0,0};
    Vec3 normal{0,0,1};
    float distance = 0.0f;
    RigidBody* body = nullptr;
};

struct DistanceJointDesc {
    RigidBody* bodyA = nullptr;
    RigidBody* bodyB = nullptr;
    Vec3 localAnchorA{0,0,0};
    Vec3 localAnchorB{0,0,0};
    float restLength = -1.0f; // negative means "use current distance"
    float stiffness = 1.0f;   // 0..1 positional correction strength
    float damping = 0.2f;     // velocity damping on joint axis
};

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void step(float dt);
    
    void addBody(RigidBody* body);
    void removeBody(RigidBody* body);

    int addDistanceJoint(const DistanceJointDesc& desc);
    void removeDistanceJoint(int jointId);
    void clearDistanceJoints();

    RaycastHit raycast(const Vec3& origin, const Vec3& direction, float maxDist);

    void setGravity(const Vec3& g) { m_gravity = g; }
    Vec3 getGravity() const { return m_gravity; }
    void setSolverIterations(int velocityIterations, int positionIterations);
    void setMaxSubSteps(int maxSubSteps);

private:
    struct OBB {
        Vec3 center{0,0,0};
        Vec3 axis[3]{{1,0,0}, {0,1,0}, {0,0,1}};
        Vec3 halfExtents{0.5f, 0.5f, 0.5f};
    };

    struct BoundingSphere {
        Vec3 center{0,0,0};
        float radius = 0.5f;
    };

    struct AABB {
        Vec3 min{0,0,0};
        Vec3 max{0,0,0};
    };

    struct DynamicTreeNode {
        AABB box{};
        int parent = -1;
        int left = -1;
        int right = -1;
        int height = 0;
        RigidBody* body = nullptr;

        bool isLeaf() const { return left == -1; }
    };

    struct ManifoldPoint {
        Vec3 point{0,0,0};
        float penetration = 0.0f;
        Vec3 rA{0,0,0};
        Vec3 rB{0,0,0};
        Vec3 tangent{1,0,0};
        float normalMass = 0.0f;
        float tangentMass = 0.0f;
        float bias = 0.0f;
        float normalImpulse = 0.0f;
        float tangentImpulse = 0.0f;
    };

    struct ContactManifold {
        RigidBody* a = nullptr;
        RigidBody* b = nullptr;
        uint64_t key = 0;
        Vec3 normal{0,1,0};
        float restitution = 0.0f;
        float staticFriction = 0.6f;
        float dynamicFriction = 0.45f;
        std::vector<ManifoldPoint> points;
    };

    struct CachedManifoldPoint {
        Vec3 point{0,0,0};
        float normalImpulse = 0.0f;
        float tangentImpulse = 0.0f;
    };

    struct ManifoldCacheEntry {
        Vec3 normal{0,1,0};
        std::array<CachedManifoldPoint, 4> points{};
        int pointCount = 0;
        uint32_t lastTouchedFrame = 0;
    };

    struct DistanceJointConstraint {
        int id = 0;
        RigidBody* a = nullptr;
        RigidBody* b = nullptr;
        Vec3 localAnchorA{0,0,0};
        Vec3 localAnchorB{0,0,0};
        float restLength = 0.0f;
        float stiffness = 1.0f;
        float damping = 0.2f;
        float accumulatedImpulse = 0.0f;
        float effectiveMass = 0.0f;
        float bias = 0.0f;
        Vec3 axis{1,0,0};
        Vec3 rA{0,0,0};
        Vec3 rB{0,0,0};
    };

    void simulate(float dt);
    void integrate(float dt);
    void applyToiCcd(
        const std::vector<std::pair<RigidBody*, RigidBody*>>& pairs,
        const std::unordered_map<RigidBody*, Vec3>& startPositions,
        const std::unordered_map<RigidBody*, Quat>& startOrientations,
        float dt
    );

    void prepareManifolds(std::vector<ContactManifold>& manifolds, float dt);
    void warmStartManifolds(std::vector<ContactManifold>& manifolds);
    void solveVelocityConstraints(std::vector<ContactManifold>& manifolds);
    void solvePositionConstraints(std::vector<ContactManifold>& manifolds);
    void cacheManifolds(const std::vector<ContactManifold>& manifolds);
    void pruneManifoldCache();

    void prepareJointConstraints(float dt);
    void warmStartJoints();
    void solveJointVelocityConstraints();
    void solveJointPositionConstraints();

    void updateSleepStates(float dt);

    int computeAdaptiveSubSteps(float dt) const;
    void updateBroadphaseTree(float dt);
    std::vector<std::pair<RigidBody*, RigidBody*>> buildBroadphasePairs() const;
    bool buildManifold(RigidBody* a, RigidBody* b, ContactManifold& outManifold) const;
    bool computeSweptSphereToi(
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
    ) const;

    int allocateTreeNode();
    void freeTreeNode(int node);
    int createLeaf(RigidBody* body, const AABB& fatBox);
    void destroyLeaf(int leaf);
    void insertLeaf(int leaf);
    void removeLeaf(int leaf);
    int selectBestSibling(const AABB& box) const;
    void refitUpward(int node);

    uint64_t makePairKey(const RigidBody* a, const RigidBody* b) const;
    OBB computeObb(const RigidBody& body, const Vec3& atPosition, const Quat& atOrientation) const;
    BoundingSphere computeBoundingSphere(const RigidBody& body, const Vec3& atPosition, const Quat& atOrientation, float motionPadding = 0.0f) const;
    AABB computeFatAabb(const RigidBody& body, float dt) const;
    static AABB mergeAabb(const AABB& a, const AABB& b);
    static bool aabbOverlap(const AABB& a, const AABB& b);
    static bool aabbContains(const AABB& a, const AABB& b);
    static float aabbPerimeter(const AABB& a);
    static bool raycastObb(const Vec3& origin, const Vec3& direction, const OBB& obb, float maxDist, float& outT, Vec3& outNormal);
    static Vec3 mulInvInertia(const RigidBody& body, const Vec3& v);

    std::vector<RigidBody*> m_bodies;
    std::vector<DistanceJointConstraint> m_joints;
    std::unordered_map<uint64_t, ManifoldCacheEntry> m_manifoldCache;
    Vec3 m_gravity{0, -9.81f, 0};
    
    const float FIXED_STEP = 1.0f / 60.0f;
    float m_accumulator = 0.0f;
    float m_maxFrameStep = 0.1f;
    float m_penetrationSlop = 0.005f;
    float m_positionCorrection = 0.75f;
    float m_restitutionThreshold = 1.0f;
    float m_baumgarte = 0.22f;
    float m_warmStartScale = 0.95f;
    int m_cachePersistenceFrames = 45;
    int m_solverIterations = 10;
    int m_positionIterations = 4;
    int m_maxSubSteps = 6;
    int m_nextJointId = 1;
    uint32_t m_frameCounter = 0;

    std::vector<DynamicTreeNode> m_treeNodes;
    std::vector<int> m_freeTreeNodes;
    std::unordered_map<RigidBody*, int> m_bodyLeaf;
    int m_treeRoot = -1;
};

}
