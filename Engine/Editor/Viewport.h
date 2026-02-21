#pragma once
#include <QOpenGLWidget>
#include <QTimer>

#include "../Graphics/Renderer.h"
#include "../Physics/PhysicsWorld.h"

#include <QMouseEvent>
#include <QKeyEvent>

namespace rex {

constexpr EntityId INVALID_ENTITY = 0xffffffff;
class Shader;
constexpr int KEY_CTRL = 1000;

class Viewport : public QOpenGLWidget {
    Q_OBJECT
public:
    enum class GizmoMode { Translate, Rotate, Scale };
    Viewport(QWidget* parent = nullptr);
    ~Viewport();

    // Editor -> Viewport API
    EntityId addCubeEntity();
    bool destroyEntity(EntityId id);
    void setSelectedEntity(EntityId id) { m_selectedEntity = id; }
    EntityId getSelectedEntity() const { return m_selectedEntity; }
    Transform* getSelectedTransform();
    bool setSelectedPosition(const Vec3& pos);
    void setMouseSensitivity(float v) { m_mouseSensitivity = v; }
    void setMoveSpeed(float v) { m_moveSpeed = v; }
    float getMouseSensitivity() const { return m_mouseSensitivity; }
    float getMoveSpeed() const { return m_moveSpeed; }
    void setShowGrid(bool v) { m_showGrid = v; }
    void setGizmoMode(GizmoMode m) { m_gizmoMode = m; }
    GizmoMode getGizmoMode() const { return m_gizmoMode; }

    // Camera
    Vec3 camPos{0, 2, -5};
    Vec3 camRot{0, 0, 0};

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
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
    float  m_mouseSensitivity = 0.08f;
    float  m_moveSpeed = 0.1f;
    bool   m_showGrid = true;
    bool   m_lookActive = false;

    // Selection + Gizmo
    EntityId m_selectedEntity = INVALID_ENTITY;
    int      m_gizmoAxis = -1; // 0=X,1=Y,2=Z
    bool     m_draggingGizmo = false;
    QPoint   m_dragStartMouse;
    Vec3     m_dragStartPos{0,0,0};
    Vec3     m_dragStartScale{1,1,1};
    Vec3     m_dragStartRot{0,0,0};
    GizmoMode m_gizmoMode = GizmoMode::Translate;

    std::unique_ptr<Shader> m_gizmoShader;
    uint32_t m_gizmoVAO = 0;
    uint32_t m_gizmoVBO = 0;
    uint32_t m_gridVAO = 0;
    uint32_t m_gridVBO = 0;
    int      m_gridVertexCount = 0;
};

}
