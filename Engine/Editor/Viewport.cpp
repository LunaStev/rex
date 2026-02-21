#include "Viewport.h"
#include "../Graphics/GLInternal.h"
#include "../Graphics/Shader.h"

#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QCursor>
#include <algorithm>
#include <cmath>
#include <vector>

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

Transform* Viewport::getSelectedTransform() {
    if (m_selectedEntity == INVALID_ENTITY)
        return nullptr;
    return m_scene.getComponent<Transform>(m_selectedEntity);
}

bool Viewport::setSelectedPosition(const Vec3& pos) {
    auto* t = getSelectedTransform();
    if (!t) return false;
    t->position = pos;
    return true;
}

bool Viewport::destroyEntity(EntityId id) {
    if (id == INVALID_ENTITY)
        return false;
    m_scene.destroyEntity(id);
    if (m_selectedEntity == id)
        m_selectedEntity = INVALID_ENTITY;
    return true;
}

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

    // Gizmo shader (simple colored lines)
    {
        std::string gv = R"(
            #version 450 core
            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec3 aColor;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 proj;

            out vec3 vColor;
            void main() {
                vColor = aColor;
                gl_Position = proj * view * model * vec4(aPos, 1.0);
            }
        )";
        std::string gf = R"(
            #version 450 core
            in vec3 vColor;
            out vec4 FragColor;
            void main() {
                FragColor = vec4(vColor, 1.0);
            }
        )";
        m_gizmoShader = std::make_unique<Shader>(gv, gf);
    }

    // Gizmo geometry (3 axes)
    {
        float verts[] = {
            0,0,0,  1,0,0,   1,0,0,  1,0,0, // X
            0,0,0,  0,1,0,   0,1,0,  0,1,0, // Y
            0,0,0,  0,0,1,   0,0,1,  0,0,1  // Z
        };

        glGenVertexArrays(1, &m_gizmoVAO);
        glGenBuffers(1, &m_gizmoVBO);
        glBindVertexArray(m_gizmoVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_gizmoVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Grid geometry (XZ plane, Unity/Godot-like square grid + colored axes)
    {
        const int half = 50;
        const float step = 1.0f;
        const int majorStep = 5;
        const float y = -0.01f;
        std::vector<float> verts;
        verts.reserve((half * 4 + 16) * 6);

        for (int i = -half; i <= half; ++i) {
            float p = i * step;
            bool axis = (i == 0);
            bool major = (i % majorStep == 0);
            float base = major ? 0.28f : 0.18f;
            float cx = base, cy = base, cz = base;

            // Lines parallel to X (vary Z)
            verts.push_back(-half * step); verts.push_back(y); verts.push_back(p);
            // Z==0 line is X axis (red)
            if (axis) { verts.push_back(0.9f); verts.push_back(0.2f); verts.push_back(0.2f); }
            else { verts.push_back(cx); verts.push_back(cy); verts.push_back(cz); }
            verts.push_back( half * step); verts.push_back(y); verts.push_back(p);
            if (axis) { verts.push_back(0.9f); verts.push_back(0.2f); verts.push_back(0.2f); }
            else { verts.push_back(cx); verts.push_back(cy); verts.push_back(cz); }
            // Lines parallel to Z (vary X)
            verts.push_back(p); verts.push_back(y); verts.push_back(-half * step);
            // X==0 line is Z axis (blue)
            if (axis) { verts.push_back(0.2f); verts.push_back(0.4f); verts.push_back(0.9f); }
            else { verts.push_back(cx); verts.push_back(cy); verts.push_back(cz); }
            verts.push_back(p); verts.push_back(y); verts.push_back( half * step);
            if (axis) { verts.push_back(0.2f); verts.push_back(0.4f); verts.push_back(0.9f); }
            else { verts.push_back(cx); verts.push_back(cy); verts.push_back(cz); }
        }

        // Y axis (green)
        verts.push_back(0.0f); verts.push_back(-half * step); verts.push_back(0.0f);
        verts.push_back(0.2f); verts.push_back(0.9f); verts.push_back(0.2f);
        verts.push_back(0.0f); verts.push_back( half * step); verts.push_back(0.0f);
        verts.push_back(0.2f); verts.push_back(0.9f); verts.push_back(0.2f);

        m_gridVertexCount = int(verts.size() / 6);
        glGenVertexArrays(1, &m_gridVAO);
        glGenBuffers(1, &m_gridVBO);
        glBindVertexArray(m_gridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    if (!m_sceneInitialized) {
        m_physics = std::make_unique<PhysicsWorld>();
        auto l = m_scene.createEntity();
        m_scene.addComponent<Transform>(l, Vec3{5, 10, 5});
        m_scene.addComponent<Light>(l, Vec3{1, 1, 1}, 1.0f, Light::Point);

        m_sceneInitialized = true;
        m_timer->start(16);
    }
}

static inline Vec3 mulPoint(const Mat4& m, const Vec3& v, float& wOut) {
    float x = m.m[0] * v.x + m.m[4] * v.y + m.m[8]  * v.z + m.m[12];
    float y = m.m[1] * v.x + m.m[5] * v.y + m.m[9]  * v.z + m.m[13];
    float z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14];
    float w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15];
    wOut = w;
    return {x, y, z};
}

static inline Vec2 projectToScreen(const Vec3& world, const Mat4& view, const Mat4& proj, int w, int h) {
    float w1 = 1.0f;
    Vec3 v1 = mulPoint(view, world, w1);
    float w2 = 1.0f;
    Vec3 v2 = mulPoint(proj, v1, w2);
    if (w2 == 0.0f) w2 = 1.0f;
    float ndcX = v2.x / w2;
    float ndcY = v2.y / w2;
    float sx = (ndcX * 0.5f + 0.5f) * float(w);
    float sy = (1.0f - (ndcY * 0.5f + 0.5f)) * float(h);
    return {sx, sy};
}

static inline float distancePointToLine(const Vec2& p, const Vec2& a, const Vec2& b) {
    float vx = b.x - a.x;
    float vy = b.y - a.y;
    float wx = p.x - a.x;
    float wy = p.y - a.y;
    float c1 = vx * wx + vy * wy;
    float c2 = vx * vx + vy * vy;
    if (c2 <= 0.0001f) return 1e9f;
    float t = c1 / c2;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float px = a.x + t * vx;
    float py = a.y + t * vy;
    float dx = p.x - px;
    float dy = p.y - py;
    return std::sqrt(dx * dx + dy * dy);
}

void Viewport::paintGL() {
    if (!m_renderer || !m_sceneInitialized)
        return;

    Camera cam;
    cam.aspect = float(width()) / float(height());

    float yaw   = camRot.y * 0.0174533f;
    float pitch = camRot.x * 0.0174533f;

    Vec3 forward{
        std::cos(pitch) * std::sin(yaw),
        std::sin(pitch),
        std::cos(pitch) * std::cos(yaw)
    };

    Mat4 view = Mat4::lookAtLH(camPos, camPos + forward, {0, 1, 0});

    m_renderer->render(
        m_scene, cam, view, camPos, width(), height(), defaultFramebufferObject(),
        [this](const Mat4& viewM, const Mat4& projM) {
            if (m_showGrid && m_gizmoShader && m_gridVAO) {
                // Grid should be depth-tested against scene
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);
                m_gizmoShader->bind();
                Mat4 model = Mat4::identity();
                m_gizmoShader->setUniform("model", model);
                m_gizmoShader->setUniform("view", viewM);
                m_gizmoShader->setUniform("proj", projM);
                glBindVertexArray(m_gridVAO);
                glDrawArrays(GL_LINES, 0, m_gridVertexCount);
                glBindVertexArray(0);
                glDepthMask(GL_TRUE);
            }

            if (m_selectedEntity != INVALID_ENTITY && m_gizmoShader && m_gizmoVAO) {
                auto* t = m_scene.getComponent<Transform>(m_selectedEntity);
                if (t) {
                    glDisable(GL_DEPTH_TEST);
                    m_gizmoShader->bind();
                    Mat4 model = Mat4::translate(t->position);
                    m_gizmoShader->setUniform("model", model);
                    m_gizmoShader->setUniform("view", viewM);
                    m_gizmoShader->setUniform("proj", projM);
                    glBindVertexArray(m_gizmoVAO);
                    glDrawArrays(GL_LINES, 0, 6);
                    glBindVertexArray(0);
                    glEnable(GL_DEPTH_TEST);
                }
            }
        });

    // Gizmo handled in extra draw callback
}

void Viewport::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void Viewport::onTick() {
    if (!m_sceneInitialized || !m_physics)
        return;

    float speed = m_moveSpeed;
    float yaw = camRot.y * 0.0174533f;

    Vec3 forward{ std::sin(yaw), 0, std::cos(yaw) };
    Vec3 right  { std::cos(yaw), 0, -std::sin(yaw) };

    if (m_keys[Qt::Key_W]) camPos = camPos + forward * speed;
    if (m_keys[Qt::Key_S]) camPos = camPos - forward * speed;
    if (m_keys[Qt::Key_A]) camPos = camPos - right * speed;
    if (m_keys[Qt::Key_D]) camPos = camPos + right * speed;
    if (m_keys[Qt::Key_Space]) camPos.y += speed;
    if (m_keys[KEY_CTRL]) camPos.y -= speed;

    m_physics->step(0.016f);
    update();
}

EntityId Viewport::addCubeEntity() {
    if (!m_sceneInitialized)
        return INVALID_ENTITY;

    // Ensure we have a current GL context before creating GPU resources
    makeCurrent();

    auto e = m_scene.createEntity();
    m_scene.addComponent<Transform>(e, camPos + Vec3{0, 0, 5});
    m_scene.addComponent<MeshRenderer>(e, nullptr, Mesh::createCube(), Vec3{1, 1, 1});

    doneCurrent();
    return e;
}

void Viewport::mousePressEvent(QMouseEvent* e) {
    m_lastMousePos = e->pos();

    if (e->button() == Qt::RightButton) {
        setCursor(Qt::BlankCursor);
        grabMouse();
        QPoint center = mapToGlobal(QPoint(width() / 2, height() / 2));
        QCursor::setPos(center);
        m_lastMousePos = mapFromGlobal(center);
    }

    if (e->button() == Qt::LeftButton && m_selectedEntity != INVALID_ENTITY) {
        Camera cam;
        cam.aspect = float(width()) / float(height());
        float yaw   = camRot.y * 0.0174533f;
        float pitch = camRot.x * 0.0174533f;

        Vec3 forward{
            std::cos(pitch) * std::sin(yaw),
            std::sin(pitch),
            std::cos(pitch) * std::cos(yaw)
        };

        Mat4 view = Mat4::lookAtLH(camPos, camPos + forward, {0, 1, 0});
        auto* t = m_scene.getComponent<Transform>(m_selectedEntity);
        if (!t) return;

        Vec2 o = projectToScreen(t->position, view, cam.getProjection(), width(), height());
        Vec2 x = projectToScreen(t->position + Vec3{1,0,0}, view, cam.getProjection(), width(), height());
        Vec2 y = projectToScreen(t->position + Vec3{0,1,0}, view, cam.getProjection(), width(), height());
        Vec2 z = projectToScreen(t->position + Vec3{0,0,1}, view, cam.getProjection(), width(), height());

        Vec2 mp{float(e->pos().x()), float(e->pos().y())};
        float dx = distancePointToLine(mp, o, x);
        float dy = distancePointToLine(mp, o, y);
        float dz = distancePointToLine(mp, o, z);

        const float threshold = 8.0f;
        m_gizmoAxis = -1;
        if (dx < threshold) m_gizmoAxis = 0;
        else if (dy < threshold) m_gizmoAxis = 1;
        else if (dz < threshold) m_gizmoAxis = 2;

        if (m_gizmoAxis != -1) {
            m_draggingGizmo = true;
            m_dragStartMouse = e->pos();
            m_dragStartPos = t->position;
            m_dragStartScale = t->scale;
            m_dragStartRot = t->rotation;
        }
    }
}

void Viewport::mouseMoveEvent(QMouseEvent* e) {
    if (m_draggingGizmo && m_selectedEntity != INVALID_ENTITY) {
        auto* t = m_scene.getComponent<Transform>(m_selectedEntity);
        if (!t) return;

        int dx = e->pos().x() - m_dragStartMouse.x();
        int dy = e->pos().y() - m_dragStartMouse.y();

        Camera cam;
        cam.aspect = float(width()) / float(height());
        float dist = std::sqrt(
            (camPos.x - m_dragStartPos.x) * (camPos.x - m_dragStartPos.x) +
            (camPos.y - m_dragStartPos.y) * (camPos.y - m_dragStartPos.y) +
            (camPos.z - m_dragStartPos.z) * (camPos.z - m_dragStartPos.z)
        );
        float unitsPerPixel = 2.0f * std::tan(cam.fov * 0.0174533f * 0.5f) * dist / float(height());
        float delta = (m_gizmoAxis == 1) ? -dy * unitsPerPixel : dx * unitsPerPixel;
        if (m_gizmoAxis == 0) delta = -delta;
        if (m_gizmoAxis == 2) delta = -delta;

        Vec3 axis = {0,0,0};
        if (m_gizmoAxis == 0) axis = {1,0,0};
        if (m_gizmoAxis == 1) axis = {0,1,0};
        if (m_gizmoAxis == 2) axis = {0,0,1};

        if (m_gizmoMode == GizmoMode::Translate) {
            t->position = {
                m_dragStartPos.x + axis.x * delta,
                m_dragStartPos.y + axis.y * delta,
                m_dragStartPos.z + axis.z * delta
            };
        } else if (m_gizmoMode == GizmoMode::Scale) {
            t->scale = {
                std::max(0.01f, m_dragStartScale.x + axis.x * delta),
                std::max(0.01f, m_dragStartScale.y + axis.y * delta),
                std::max(0.01f, m_dragStartScale.z + axis.z * delta)
            };
        } else if (m_gizmoMode == GizmoMode::Rotate) {
            float deg = delta * 30.0f;
            t->rotation = {
                m_dragStartRot.x + axis.x * deg,
                m_dragStartRot.y + axis.y * deg,
                m_dragStartRot.z + axis.z * deg
            };
        }
        update();
        return;
    }

    if (e->buttons() & Qt::RightButton) {
        float sensitivity = m_mouseSensitivity;
        QPoint center = mapToGlobal(QPoint(width() / 2, height() / 2));
        QPoint current = QCursor::pos();
        int dx = current.x() - center.x();
        int dy = current.y() - center.y();
        camRot.y += dx * sensitivity;
        camRot.x -= dy * sensitivity;
        camRot.x = std::clamp(camRot.x, -89.0f, 89.0f);
        QCursor::setPos(center);
    }
    m_lastMousePos = e->pos();
}

void Viewport::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() == Qt::RightButton) {
        releaseMouse();
        unsetCursor();
    }
    Q_UNUSED(e);
    m_draggingGizmo = false;
    m_gizmoAxis = -1;
}

void Viewport::keyPressEvent(QKeyEvent* e) {
    int k = e->key();
    if (k == Qt::Key_Control) k = KEY_CTRL;
    if (k >= 0 && k < 1024)
        m_keys[k] = true;

    if (k == Qt::Key_W) m_gizmoMode = GizmoMode::Translate;
    if (k == Qt::Key_E) m_gizmoMode = GizmoMode::Rotate;
    if (k == Qt::Key_R) m_gizmoMode = GizmoMode::Scale;
}

void Viewport::keyReleaseEvent(QKeyEvent* e) {
    int k = e->key();
    if (k == Qt::Key_Control) k = KEY_CTRL;
    if (k >= 0 && k < 1024)
        m_keys[k] = false;
}

}
