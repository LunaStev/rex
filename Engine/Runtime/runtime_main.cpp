#include "../Core/Window.h"
#include "../Core/Logger.h"
#include "../Core/Scene.h"
#include "../Core/Components.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Mesh.h"
#include "../Physics/PhysicsSystem.h"

int main() {
    using namespace rex;
    
    Window window({ .title = "Rex Engine - Graphics & Physics", .width = 1280, .height = 720 });
    Renderer renderer;
    PhysicsSystem physics;
    Mesh* cube = Mesh::createCube();
    
    Scene scene;
    
    // Light
    auto light = scene.createEntity();
    scene.addComponent<Transform>(light, Vec3{5, 10, 2});
    scene.addComponent<Light>(light, Vec3{1, 1, 1}, 1.2f, Light::Point);

    // Falling Cubes
    for(int i = 0; i < 5; ++i) {
        auto entity = scene.createEntity();
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
