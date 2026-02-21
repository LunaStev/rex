#include "Viewport.h"
#include "../Graphics/GLInternal.h"

#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <algorithm>
#include <cmath>

namespace rex {

Viewport::Viewport(QWidget* parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Viewport::onTick);
}

Viewport::~Viewport() = default;

void Viewport::initializeGL() {
    auto* ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        std::cerr << "[Viewport] Error: No OpenGL context current in initializeGL\n";
        return;
    }

    loadGLFunctions([](const char* name) -> void* {
        auto* c = QOpenGLContext::currentContext();
        return c ? (void*)c->getProcAddress(name) : nullptr;
    });

    if (!glCreateProgram) {
        std::cerr << "[Viewport] Error: Failed to load core GL functions. Graphics will fail.\n";
        return;
    }

    glEnable(GL_DEPTH_TEST);

    m_renderer = std::make_unique<Renderer>();

    if (!m_sceneInitialized) {
        m_physics = std::make_unique<PhysicsWorld>();
        m_debugCube = Mesh::createCube();

        auto e = m_scene.createEntity();
        m_scene.addComponent<Transform>(e, Vec3{0, 0, 0});
        m_scene.addComponent<MeshRenderer>(e, nullptr, m_debugCube, Vec3{1, 0.5f, 0.2f});

        auto l = m_scene.createEntity();
        m_scene.addComponent<Transform>(l, Vec3{5, 10, 5});
        m_scene.addComponent<Light>(l, Vec3{1, 1, 1}, 1.0f, Light::Point);

        m_sceneInitialized = true;
        m_timer->start(16);
    }
}

void Viewport::paintGL() {
    if (!m_renderer || !m_sceneInitialized)
        return;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera cam;
    cam.aspect = float(width()) / float(height());

    float yaw   = camRot.y * 0.0174533f;
    float pitch = camRot.x * 0.0174533f;

    Vec3 forward{
        std::cos(pitch) * std::sin(yaw),
        -std::sin(pitch),
        -std::cos(pitch) * std::cos(yaw)
    };

    Mat4 view = Mat4::lookAt(camPos, camPos + forward, {0, 1, 0});
    m_renderer->render(m_scene, cam, view);
}

void Viewport::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void Viewport::onTick() {
    if (!m_sceneInitialized || !m_physics)
        return;

    constexpr float speed = 0.1f;
    float yaw = camRot.y * 0.0174533f;

    Vec3 forward{ std::sin(yaw), 0, -std::cos(yaw) };
    Vec3 right  { std::cos(yaw), 0,  std::sin(yaw) };

    if (m_keys[Qt::Key_W]) camPos = camPos + forward * speed;
    if (m_keys[Qt::Key_S]) camPos = camPos - forward * speed;
    if (m_keys[Qt::Key_A]) camPos = camPos - right * speed;
    if (m_keys[Qt::Key_D]) camPos = camPos + right * speed;
    if (m_keys[Qt::Key_Space]) camPos.y += speed;
    if (m_keys[Qt::Key_Control]) camPos.y -= speed;

    m_physics->step(0.016f);
    update();
}

EntityId Viewport::addCubeEntity() {
    if (!m_sceneInitialized)
        return INVALID_ENTITY;

    auto e = m_scene.createEntity();
    m_scene.addComponent<Transform>(e, camPos + Vec3{0, 0, -5});
    m_scene.addComponent<MeshRenderer>(e, nullptr, Mesh::createCube(), Vec3{1, 1, 1});
    return e;
}

void Viewport::mousePressEvent(QMouseEvent* e) {
    m_lastMousePos = e->pos();
}

void Viewport::mouseMoveEvent(QMouseEvent* e) {
    if (e->buttons() & Qt::RightButton) {
        constexpr float sensitivity = 0.2f;
        camRot.y += (e->pos().x() - m_lastMousePos.x()) * sensitivity;
        camRot.x += (e->pos().y() - m_lastMousePos.y()) * sensitivity;
        camRot.x = std::clamp(camRot.x, -89.0f, 89.0f);
    }
    m_lastMousePos = e->pos();
}

void Viewport::keyPressEvent(QKeyEvent* e) {
    if (e->key() < 1024)
        m_keys[e->key()] = true;
}

void Viewport::keyReleaseEvent(QKeyEvent* e) {
    if (e->key() < 1024)
        m_keys[e->key()] = false;
}

}