#include "../Core/Window.h"
#include "../Core/Logger.h"
#include "../Core/Scene.h"
#include "../Core/Components.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Mesh.h"
#include "../Physics/PhysicsSystem.h"
#include <vector>

int main() {
    using namespace rex;
    
    Window window({ .title = "Rex Engine - Graphics & Physics", .width = 1280, .height = 720 });
    Renderer renderer;
    PhysicsSystem physics;
    physics.getWorld().setSolverIterations(12, 5);
    physics.getWorld().setMaxSubSteps(8);
    Mesh* cube = Mesh::createCube();
    
    Scene scene;
    
    // Light
    auto light = scene.createEntity();
    scene.addComponent<Transform>(light, Vec3{5, 10, 2});
    scene.addComponent<Light>(light, Vec3{1, 1, 1}, 1.2f, Light::Point);

    // Falling Cubes
    std::vector<EntityId> dynamicCubes;
    dynamicCubes.reserve(5);
    for(int i = 0; i < 5; ++i) {
        auto entity = scene.createEntity();
        dynamicCubes.push_back(entity);
        scene.addComponent<Transform>(entity, Vec3{(float)i*1.5f - 3.0f, 10.0f + (float)i*2.0f, -10});
        scene.addComponent<MeshRenderer>(entity, nullptr, cube, Vec3{0.8f, 0.2f, 0.2f});
        scene.addComponent<RigidBodyComponent>(entity, BodyType::Dynamic, 1.0f);
    }
    
    // Ground
    auto ground = scene.createEntity();
    auto& gt = scene.addComponent<Transform>(ground, Vec3{0, -2, -10});
    gt.scale = {20, 1, 20};
    scene.addComponent<MeshRenderer>(ground, nullptr, cube, Vec3{0.4f, 0.4f, 0.4f});
    scene.addComponent<RigidBodyComponent>(ground, BodyType::Static, 0.0f);

    // Build physics bodies once, then add a sample joint constraint.
    physics.update(scene, 0.0f);
    if (dynamicCubes.size() >= 2) {
        auto* rb0 = scene.getComponent<RigidBodyComponent>(dynamicCubes[0]);
        auto* rb1 = scene.getComponent<RigidBodyComponent>(dynamicCubes[1]);
        if (rb0 && rb1 && rb0->internalBody && rb1->internalBody) {
            DistanceJointDesc joint;
            joint.bodyA = rb0->internalBody;
            joint.bodyB = rb1->internalBody;
            joint.restLength = 2.0f;
            joint.stiffness = 0.7f;
            joint.damping = 0.15f;
            physics.getWorld().addDistanceJoint(joint);
        }
    }
    
    Camera cam;
    cam.aspect = float(window.getWidth()) / float(window.getHeight());
    Vec3 camPos{0, 2, -10};
    
    while (!window.shouldClose()) {
        window.pollEvents();
        
        physics.update(scene, 1.0f / 60.0f);

        Mat4 view = Mat4::lookAtLH(camPos, Vec3{0, 0, 0}, Vec3{0, 1, 0});
        renderer.render(scene, cam, view, camPos, window.getWidth(), window.getHeight(), 0);
        
        window.swapBuffers();
    }
    
    delete cube;
    return 0;
}
