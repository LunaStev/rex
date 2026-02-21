#pragma once
#include <QOpenGLWidget>
#include <QTimer>

#include "../Graphics/Renderer.h"
#include "../Physics/PhysicsWorld.h"

#include <QMouseEvent>
#include <QKeyEvent>

namespace rex {

constexpr EntityId INVALID_ENTITY = 0xffffffff;

class Viewport : public QOpenGLWidget {
    Q_OBJECT
public:
    Viewport(QWidget* parent = nullptr);
    ~Viewport();

    // Editor -> Viewport API
    EntityId addCubeEntity();

    // Camera
    Vec3 camPos{0, 2, 5};
    Vec3 camRot{0, 0, 0};

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void onTick();

private:
    std::unique_ptr<Renderer>     m_renderer;
    std::unique_ptr<PhysicsWorld> m_physics;

    Scene   m_scene;
    QTimer* m_timer = nullptr;

    Mesh* m_debugCube = nullptr;
    bool  m_sceneInitialized = false;

    QPoint m_lastMousePos;
    bool   m_keys[1024]{false};
};

}