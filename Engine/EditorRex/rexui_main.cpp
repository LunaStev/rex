#include "../Core/Components.h"
#include "../Core/Logger.h"
#include "../Core/Scene.h"
#include "../Graphics/GLInternal.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Renderer.h"
#include "../UI/RexUI.h"
#include "../UI/RexUIRenderer.h"
#include "../UI/RexUIWidgets.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <deque>
#include <sstream>
#include <string>
#include <vector>

using namespace rex;
using namespace rex::ui;

namespace {
constexpr EntityId INVALID_ENTITY = 0xffffffffu;
constexpr float DEG2RAD = 0.01745329251994329577f;

enum class GizmoMode {
    Select,
    Translate,
    Rotate,
    Scale
};

struct EditorUIRefs {
    TreeView* hierarchyTree = nullptr;
    TextBox* hierarchySearch = nullptr;

    Label* selectedLabel = nullptr;
    Label* modeLabel = nullptr;
    Label* cameraLabel = nullptr;

    TextBox* posBox = nullptr;
    TextBox* rotBox = nullptr;
    TextBox* scaleBox = nullptr;

    TextBox* sensitivityBox = nullptr;
    TextBox* moveSpeedBox = nullptr;

    Label* outputLabel = nullptr;
    Label* viewportLabel = nullptr;
};

struct EditorState {
    Scene scene;
    Renderer renderer;
    Camera camera;
    Mesh* cubeMesh = nullptr;

    Vec3 camPos{0.0f, 2.0f, -6.0f};
    Vec3 camRot{12.0f, 0.0f, 0.0f};
    float mouseSensitivity = 0.08f;
    float moveSpeed = 4.0f;

    EntityId selected = INVALID_ENTITY;
    GizmoMode gizmoMode = GizmoMode::Translate;

    bool running = true;
    bool lookActive = false;
    bool hierarchyDirty = true;
    bool sceneTabActive = true;
    std::string hierarchyFilter;

    std::deque<std::string> logs;
};

std::string toLowerCopy(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

std::string gizmoModeName(GizmoMode mode) {
    switch (mode) {
        case GizmoMode::Select: return "Select";
        case GizmoMode::Translate: return "Translate";
        case GizmoMode::Rotate: return "Rotate";
        case GizmoMode::Scale: return "Scale";
    }
    return "Unknown";
}

void pushLog(EditorState& state, const std::string& line) {
    state.logs.push_back(line);
    while (state.logs.size() > 12) {
        state.logs.pop_front();
    }
}

bool parseVec3(const std::string& s, Vec3& out) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    if (std::sscanf(s.c_str(), " %f , %f , %f ", &x, &y, &z) == 3) {
        out = {x, y, z};
        return true;
    }
    return false;
}

bool parseFloat(const std::string& s, float& out) {
    std::stringstream ss(s);
    ss >> out;
    return !ss.fail() && ss.eof();
}

std::string formatVec3(const Vec3& v) {
    char buf[128];
    std::snprintf(buf, sizeof(buf), "%.3f, %.3f, %.3f", v.x, v.y, v.z);
    return std::string(buf);
}

std::string formatFloat(float v) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.3f", v);
    return std::string(buf);
}

EntityId addCubeEntity(EditorState& state, const Vec3& position) {
    EntityId e = state.scene.createEntity();
    state.scene.addComponent<Transform>(e, position);
    state.scene.addComponent<MeshRenderer>(e, nullptr, state.cubeMesh, Vec3{1.0f, 1.0f, 1.0f});
    state.hierarchyDirty = true;
    pushLog(state, "Add Cube: Entity " + std::to_string(e));
    return e;
}

bool deleteSelectedEntity(EditorState& state) {
    if (state.selected == INVALID_ENTITY) {
        pushLog(state, "Delete skipped: no selected entity");
        return false;
    }

    if (!state.scene.getComponent<Transform>(state.selected)) {
        state.selected = INVALID_ENTITY;
        state.hierarchyDirty = true;
        pushLog(state, "Delete skipped: stale selection");
        return false;
    }

    const EntityId id = state.selected;
    state.scene.destroyEntity(id);
    state.selected = INVALID_ENTITY;
    state.hierarchyDirty = true;
    pushLog(state, "Delete Entity: " + std::to_string(id));
    return true;
}

void refreshHierarchy(EditorState& state, EditorUIRefs& uiRefs) {
    if (!uiRefs.hierarchyTree) {
        return;
    }

    if (!state.hierarchyDirty) {
        return;
    }

    std::vector<TreeView::Node> nodes;
    const std::string filterLower = toLowerCopy(state.hierarchyFilter);

    bool selectedStillValid = false;
    state.scene.each<MeshRenderer>([&](EntityId id, MeshRenderer&) {
        if (!state.scene.getComponent<Transform>(id)) {
            return;
        }

        const std::string label = "Entity " + std::to_string(id);
        if (!filterLower.empty()) {
            if (toLowerCopy(label).find(filterLower) == std::string::npos) {
                return;
            }
        }

        TreeView::Node n;
        n.id = static_cast<int>(id);
        n.label = label;
        nodes.push_back(std::move(n));

        if (id == state.selected) {
            selectedStillValid = true;
        }
    });

    std::sort(nodes.begin(), nodes.end(), [](const TreeView::Node& a, const TreeView::Node& b) {
        return a.id < b.id;
    });

    if (nodes.empty()) {
        TreeView::Node empty;
        empty.id = -1;
        empty.label = "(No entities)";
        empty.expanded = false;
        nodes.push_back(std::move(empty));
    }

    if (!selectedStillValid && state.selected != INVALID_ENTITY) {
        state.selected = INVALID_ENTITY;
    }

    uiRefs.hierarchyTree->setNodes(std::move(nodes));
    state.hierarchyDirty = false;
}

void syncInspectorFromSelection(EditorState& state, UIContext& ui, EditorUIRefs& uiRefs) {
    auto* tr = state.selected == INVALID_ENTITY ? nullptr : state.scene.getComponent<Transform>(state.selected);

    if (uiRefs.selectedLabel) {
        if (tr) {
            uiRefs.selectedLabel->setText("Selected: Entity " + std::to_string(state.selected));
        } else {
            uiRefs.selectedLabel->setText("Selected: None");
        }
    }

    if (uiRefs.modeLabel) {
        uiRefs.modeLabel->setText("Gizmo: " + gizmoModeName(state.gizmoMode));
    }

    if (uiRefs.cameraLabel) {
        uiRefs.cameraLabel->setText(
            "Camera Pos " + formatVec3(state.camPos) + " Rot " + formatVec3(state.camRot));
    }

    if (uiRefs.viewportLabel) {
        uiRefs.viewportLabel->setText(
            state.sceneTabActive ? "Viewport: Scene" : "Viewport: Game");
    }

    if (uiRefs.outputLabel) {
        if (state.logs.empty()) {
            uiRefs.outputLabel->setText("Output: Ready");
        } else {
            uiRefs.outputLabel->setText("Output: " + state.logs.back());
        }
    }

    Widget* focused = ui.focused();
    if (uiRefs.sensitivityBox && focused != uiRefs.sensitivityBox) uiRefs.sensitivityBox->setText(formatFloat(state.mouseSensitivity));
    if (uiRefs.moveSpeedBox && focused != uiRefs.moveSpeedBox) uiRefs.moveSpeedBox->setText(formatFloat(state.moveSpeed));

    if (!tr) {
        if (uiRefs.posBox && focused != uiRefs.posBox) uiRefs.posBox->setText("0, 0, 0");
        if (uiRefs.rotBox && focused != uiRefs.rotBox) uiRefs.rotBox->setText("0, 0, 0");
        if (uiRefs.scaleBox && focused != uiRefs.scaleBox) uiRefs.scaleBox->setText("1, 1, 1");
        return;
    }

    if (uiRefs.posBox && focused != uiRefs.posBox) uiRefs.posBox->setText(formatVec3(tr->position));
    if (uiRefs.rotBox && focused != uiRefs.rotBox) uiRefs.rotBox->setText(formatVec3(tr->rotation));
    if (uiRefs.scaleBox && focused != uiRefs.scaleBox) uiRefs.scaleBox->setText(formatVec3(tr->scale));
}

int toUIKey(SDL_Keycode key) {
    switch (key) {
        case SDLK_BACKSPACE: return static_cast<int>(UIKey::Backspace);
        case SDLK_RETURN:
        case SDLK_KP_ENTER: return static_cast<int>(UIKey::Enter);
        case SDLK_DELETE: return static_cast<int>(UIKey::DeleteKey);
        case SDLK_LEFT: return static_cast<int>(UIKey::Left);
        case SDLK_RIGHT: return static_cast<int>(UIKey::Right);
        default:
            if (key >= 0 && key <= 255) {
                return static_cast<int>(key);
            }
            return -1;
    }
}

std::unique_ptr<Button> makeButton(const std::string& title, float width, const std::function<void()>& onClick) {
    auto b = std::make_unique<Button>(title);
    b->setPreferredSize({width, 24.0f});
    b->setOnClick(onClick);
    return b;
}

void buildEditorUI(UIContext& ui, EditorState& state, EditorUIRefs& refs) {
    auto root = std::make_unique<Panel>();
    root->setLayout(UILayout::Column);
    root->setAlign(UIAlign::Stretch);
    root->setStyle({{0.08f, 0.08f, 0.09f, 0.94f}, {0,0,0,0}, {1,1,1,1}, 4, 0, 0});

    auto top = std::make_unique<Panel>();
    top->setLayout(UILayout::Column);
    top->setAlign(UIAlign::Stretch);
    top->setPreferredSize({0, 58});

    auto menuRow = std::make_unique<Panel>();
    menuRow->setLayout(UILayout::Row);
    menuRow->setPreferredSize({0, 26});
    menuRow->setStyle({{0.12f, 0.12f, 0.13f, 1.0f}, {0,0,0,0}, {1,1,1,1}, 2, 0, 0});

    menuRow->addChild(makeButton("Load Model", 100.0f, [&]() {
        pushLog(state, "Load Model: TODO (native file dialog not wired yet)");
    }));
    menuRow->addChild(makeButton("Add Cube", 90.0f, [&]() {
        state.selected = addCubeEntity(state, state.camPos + Vec3{0.0f, 0.0f, 5.0f});
    }));
    menuRow->addChild(makeButton("Delete", 80.0f, [&]() {
        deleteSelectedEntity(state);
    }));
    menuRow->addChild(makeButton("Scene", 70.0f, [&]() {
        state.sceneTabActive = true;
    }));
    menuRow->addChild(makeButton("Game", 70.0f, [&]() {
        state.sceneTabActive = false;
    }));

    auto toolbarRow = std::make_unique<Panel>();
    toolbarRow->setLayout(UILayout::Row);
    toolbarRow->setPreferredSize({0, 26});
    toolbarRow->setStyle({{0.10f, 0.10f, 0.11f, 1.0f}, {0,0,0,0}, {1,1,1,1}, 2, 0, 0});

    toolbarRow->addChild(makeButton("Select", 80.0f, [&]() {
        state.gizmoMode = GizmoMode::Select;
        pushLog(state, "Gizmo mode: Select");
    }));
    toolbarRow->addChild(makeButton("Move", 80.0f, [&]() {
        state.gizmoMode = GizmoMode::Translate;
        pushLog(state, "Gizmo mode: Translate");
    }));
    toolbarRow->addChild(makeButton("Rotate", 80.0f, [&]() {
        state.gizmoMode = GizmoMode::Rotate;
        pushLog(state, "Gizmo mode: Rotate");
    }));
    toolbarRow->addChild(makeButton("Scale", 80.0f, [&]() {
        state.gizmoMode = GizmoMode::Scale;
        pushLog(state, "Gizmo mode: Scale");
    }));

    top->addChild(std::move(menuRow));
    top->addChild(std::move(toolbarRow));
    root->addChild(std::move(top));

    auto body = std::make_unique<Panel>();
    body->setLayout(UILayout::Row);
    body->setAlign(UIAlign::Stretch);
    body->setFlex(1.0f);

    auto hierarchyPanel = std::make_unique<Panel>();
    hierarchyPanel->setLayout(UILayout::Column);
    hierarchyPanel->setAlign(UIAlign::Stretch);
    hierarchyPanel->setPreferredSize({310.0f, 0});
    hierarchyPanel->addChild(std::make_unique<Label>("Hierarchy"));

    auto search = std::make_unique<TextBox>();
    refs.hierarchySearch = search.get();
    search->setPreferredSize({0, 24});
    search->setOnCommit([&](const std::string& text) {
        state.hierarchyFilter = text;
        state.hierarchyDirty = true;
        pushLog(state, "Hierarchy filter: " + text);
    });
    hierarchyPanel->addChild(std::move(search));

    auto hierarchyButtons = std::make_unique<Panel>();
    hierarchyButtons->setLayout(UILayout::Row);
    hierarchyButtons->setPreferredSize({0, 24});
    hierarchyButtons->addChild(makeButton("Add Cube", 120.0f, [&]() {
        state.selected = addCubeEntity(state, state.camPos + Vec3{0.0f, 0.0f, 5.0f});
    }));
    hierarchyButtons->addChild(makeButton("Delete", 100.0f, [&]() {
        deleteSelectedEntity(state);
    }));
    hierarchyPanel->addChild(std::move(hierarchyButtons));

    auto tree = std::make_unique<TreeView>();
    refs.hierarchyTree = tree.get();
    tree->setFlex(1.0f);
    tree->setOnSelect([&](int id) {
        if (id < 0) return;
        state.selected = static_cast<EntityId>(id);
        pushLog(state, "Select Entity: " + std::to_string(state.selected));
    });
    hierarchyPanel->addChild(std::move(tree));

    auto centerPanel = std::make_unique<Panel>();
    centerPanel->setLayout(UILayout::Column);
    centerPanel->setAlign(UIAlign::Stretch);
    centerPanel->setFlex(1.0f);

    auto centerTabs = std::make_unique<TabView>();
    centerTabs->setFlex(1.0f);

    {
        auto sceneTab = std::make_unique<Panel>();
        sceneTab->setLayout(UILayout::Column);
        sceneTab->setAlign(UIAlign::Stretch);

        auto viewportLabel = std::make_unique<Label>("Viewport: Scene");
        refs.viewportLabel = viewportLabel.get();
        sceneTab->addChild(std::move(viewportLabel));
        sceneTab->addChild(std::make_unique<Label>("RMB: look around | WASD/Space/Ctrl: move camera"));
        sceneTab->addChild(std::make_unique<Label>("W/E/R: Translate/Rotate/Scale mode"));

        centerTabs->addTab("Scene", std::move(sceneTab));
    }

    {
        auto gameTab = std::make_unique<Panel>();
        gameTab->setLayout(UILayout::Column);
        gameTab->setAlign(UIAlign::Stretch);
        gameTab->addChild(std::make_unique<Label>("Game View"));
        gameTab->addChild(std::make_unique<Label>("Runtime preview placeholder (RexUI)"));
        centerTabs->addTab("Game", std::move(gameTab));
    }

    centerPanel->addChild(std::move(centerTabs));

    auto detailsPanel = std::make_unique<Panel>();
    detailsPanel->setLayout(UILayout::Column);
    detailsPanel->setAlign(UIAlign::Stretch);
    detailsPanel->setPreferredSize({330.0f, 0});

    detailsPanel->addChild(std::make_unique<Label>("Details"));

    auto selectedLabel = std::make_unique<Label>("Selected: None");
    refs.selectedLabel = selectedLabel.get();
    detailsPanel->addChild(std::move(selectedLabel));

    auto modeLabel = std::make_unique<Label>("Gizmo: Translate");
    refs.modeLabel = modeLabel.get();
    detailsPanel->addChild(std::move(modeLabel));

    auto cameraLabel = std::make_unique<Label>("Camera Pos 0,0,0 Rot 0,0,0");
    refs.cameraLabel = cameraLabel.get();
    detailsPanel->addChild(std::move(cameraLabel));

    detailsPanel->addChild(std::make_unique<Label>("Position (x, y, z)"));
    auto pos = std::make_unique<TextBox>();
    refs.posBox = pos.get();
    pos->setPreferredSize({0, 22});
    detailsPanel->addChild(std::move(pos));

    detailsPanel->addChild(std::make_unique<Label>("Rotation (x, y, z)"));
    auto rot = std::make_unique<TextBox>();
    refs.rotBox = rot.get();
    rot->setPreferredSize({0, 22});
    detailsPanel->addChild(std::move(rot));

    detailsPanel->addChild(std::make_unique<Label>("Scale (x, y, z)"));
    auto scale = std::make_unique<TextBox>();
    refs.scaleBox = scale.get();
    scale->setPreferredSize({0, 22});
    detailsPanel->addChild(std::move(scale));

    detailsPanel->addChild(makeButton("Apply Transform", 140.0f, [&]() {
        if (state.selected == INVALID_ENTITY) {
            pushLog(state, "Apply Transform skipped: no selection");
            return;
        }

        auto* tr = state.scene.getComponent<Transform>(state.selected);
        if (!tr) {
            pushLog(state, "Apply Transform failed: transform missing");
            return;
        }

        Vec3 p{}, r{}, s{};
        if (!refs.posBox || !parseVec3(refs.posBox->text(), p)) {
            pushLog(state, "Apply Transform failed: invalid position format");
            return;
        }
        if (!refs.rotBox || !parseVec3(refs.rotBox->text(), r)) {
            pushLog(state, "Apply Transform failed: invalid rotation format");
            return;
        }
        if (!refs.scaleBox || !parseVec3(refs.scaleBox->text(), s)) {
            pushLog(state, "Apply Transform failed: invalid scale format");
            return;
        }

        tr->position = p;
        tr->rotation = r;
        tr->scale = {
            std::max(0.01f, s.x),
            std::max(0.01f, s.y),
            std::max(0.01f, s.z)
        };
        pushLog(state, "Apply Transform: Entity " + std::to_string(state.selected));
    }));

    detailsPanel->addChild(std::make_unique<Label>("Camera Sensitivity"));
    auto sensitivity = std::make_unique<TextBox>();
    refs.sensitivityBox = sensitivity.get();
    detailsPanel->addChild(std::move(sensitivity));

    detailsPanel->addChild(std::make_unique<Label>("Camera Move Speed"));
    auto moveSpeed = std::make_unique<TextBox>();
    refs.moveSpeedBox = moveSpeed.get();
    detailsPanel->addChild(std::move(moveSpeed));

    detailsPanel->addChild(makeButton("Apply Camera Settings", 170.0f, [&]() {
        float s = state.mouseSensitivity;
        float m = state.moveSpeed;

        if (refs.sensitivityBox && !parseFloat(refs.sensitivityBox->text(), s)) {
            pushLog(state, "Settings failed: sensitivity is not a number");
            return;
        }
        if (refs.moveSpeedBox && !parseFloat(refs.moveSpeedBox->text(), m)) {
            pushLog(state, "Settings failed: move speed is not a number");
            return;
        }

        state.mouseSensitivity = std::clamp(s, 0.01f, 1.0f);
        state.moveSpeed = std::clamp(m, 0.1f, 30.0f);
        pushLog(state, "Camera settings applied");
    }));

    body->addChild(std::move(hierarchyPanel));
    body->addChild(std::move(centerPanel));
    body->addChild(std::move(detailsPanel));
    root->addChild(std::move(body));

    auto bottom = std::make_unique<Panel>();
    bottom->setLayout(UILayout::Column);
    bottom->setAlign(UIAlign::Stretch);
    bottom->setPreferredSize({0, 150});

    auto bottomTabs = std::make_unique<TabView>();
    bottomTabs->setFlex(1.0f);

    {
        auto contentTab = std::make_unique<Panel>();
        contentTab->setLayout(UILayout::Column);
        contentTab->addChild(std::make_unique<Label>("Content Browser"));
        contentTab->addChild(std::make_unique<Label>("Asset explorer migration target (RexUI)"));
        bottomTabs->addTab("Content", std::move(contentTab));
    }

    {
        auto outputTab = std::make_unique<Panel>();
        outputTab->setLayout(UILayout::Column);
        outputTab->addChild(std::make_unique<Label>("Output Log"));

        auto output = std::make_unique<Label>("Output: Ready");
        refs.outputLabel = output.get();
        outputTab->addChild(std::move(output));

        bottomTabs->addTab("Output", std::move(outputTab));
    }

    bottom->addChild(std::move(bottomTabs));
    root->addChild(std::move(bottom));

    ui.setRoot(std::move(root));
}

void updateCamera(EditorState& state, float dt) {
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    if (!keys) {
        return;
    }

    const float speedMul = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT] ? 2.5f : 1.0f;
    const float speed = state.moveSpeed * speedMul;

    const float yaw = state.camRot.y * DEG2RAD;
    Vec3 forward{std::sin(yaw), 0.0f, std::cos(yaw)};
    Vec3 right{std::cos(yaw), 0.0f, -std::sin(yaw)};

    if (keys[SDL_SCANCODE_W]) state.camPos += forward * (speed * dt);
    if (keys[SDL_SCANCODE_S]) state.camPos -= forward * (speed * dt);
    if (keys[SDL_SCANCODE_A]) state.camPos -= right * (speed * dt);
    if (keys[SDL_SCANCODE_D]) state.camPos += right * (speed * dt);
    if (keys[SDL_SCANCODE_SPACE]) state.camPos.y += speed * dt;
    if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) state.camPos.y -= speed * dt;
}

} // namespace

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Logger::error("SDL Init Error: {}", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(
        "Rex Editor",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1600,
        900,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
    );

    if (!window) {
        Logger::error("Window Error: {}", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext gl = SDL_GL_CreateContext(window);
    if (!gl) {
        Logger::error("GL Context Error: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    loadGLFunctionsSDL();

    EditorState state;
    state.camera.aspect = 16.0f / 9.0f;
    state.cubeMesh = Mesh::createCube();

    auto lightEntity = state.scene.createEntity();
    state.scene.addComponent<Transform>(lightEntity, Vec3{5.0f, 10.0f, 5.0f});
    state.scene.addComponent<Light>(lightEntity, Vec3{1.0f, 1.0f, 1.0f}, 1.0f, Light::Point);

    state.selected = addCubeEntity(state, Vec3{0.0f, 0.0f, 0.0f});
    pushLog(state, "RexUI Editor initialized (Qt removed)");

    UIContext ui;
    EditorUIRefs uiRefs;
    buildEditorUI(ui, state, uiRefs);

    RexUIRenderer uiRenderer;
    SDL_StartTextInput();

    uint64_t lastTicks = SDL_GetTicks64();
    while (state.running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                state.running = false;
                break;
            }

            if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                if (ev.window.data2 > 0) {
                    state.camera.aspect = static_cast<float>(ev.window.data1) / static_cast<float>(ev.window.data2);
                }
            }

            if (ev.type == SDL_KEYDOWN) {
                const int uiKey = toUIKey(ev.key.keysym.sym);
                bool consumed = false;
                if (uiKey >= 0) {
                    consumed = ui.dispatchKeyDown(uiKey);
                }

                if (!consumed) {
                    if (ev.key.keysym.sym == SDLK_w) {
                        state.gizmoMode = GizmoMode::Translate;
                    } else if (ev.key.keysym.sym == SDLK_e) {
                        state.gizmoMode = GizmoMode::Rotate;
                    } else if (ev.key.keysym.sym == SDLK_r) {
                        state.gizmoMode = GizmoMode::Scale;
                    } else if (ev.key.keysym.sym == SDLK_q) {
                        state.gizmoMode = GizmoMode::Select;
                    } else if (ev.key.keysym.sym == SDLK_DELETE) {
                        deleteSelectedEntity(state);
                    } else if (ev.key.keysym.sym == SDLK_ESCAPE && state.lookActive) {
                        state.lookActive = false;
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    }
                }
            }

            if (ev.type == SDL_TEXTINPUT) {
                ui.dispatchTextInput(ev.text.text);
            }

            if (ev.type == SDL_MOUSEBUTTONDOWN) {
                Vec2 p{static_cast<float>(ev.button.x), static_cast<float>(ev.button.y)};
                const int btn = ev.button.button == SDL_BUTTON_LEFT ? 0 : 1;
                const bool consumed = ui.dispatchMouseDown(p, btn);

                if (ev.button.button == SDL_BUTTON_RIGHT && !consumed) {
                    state.lookActive = true;
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                }
            }

            if (ev.type == SDL_MOUSEBUTTONUP) {
                Vec2 p{static_cast<float>(ev.button.x), static_cast<float>(ev.button.y)};
                const int btn = ev.button.button == SDL_BUTTON_LEFT ? 0 : 1;
                ui.dispatchMouseUp(p, btn);

                if (ev.button.button == SDL_BUTTON_RIGHT) {
                    state.lookActive = false;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                }
            }

            if (ev.type == SDL_MOUSEMOTION) {
                if (state.lookActive) {
                    state.camRot.y += static_cast<float>(ev.motion.xrel) * state.mouseSensitivity;
                    state.camRot.x -= static_cast<float>(ev.motion.yrel) * state.mouseSensitivity;
                    state.camRot.x = std::clamp(state.camRot.x, -89.0f, 89.0f);
                } else {
                    ui.dispatchMouseMove({static_cast<float>(ev.motion.x), static_cast<float>(ev.motion.y)});
                }
            }
        }

        const uint64_t nowTicks = SDL_GetTicks64();
        float dt = static_cast<float>(nowTicks - lastTicks) / 1000.0f;
        lastTicks = nowTicks;
        dt = std::clamp(dt, 0.0f, 0.1f);

        updateCamera(state, dt);
        refreshHierarchy(state, uiRefs);

        if (state.selected != INVALID_ENTITY && !state.scene.getComponent<Transform>(state.selected)) {
            state.selected = INVALID_ENTITY;
        }

        int w = 0;
        int h = 0;
        SDL_GetWindowSize(window, &w, &h);
        if (h <= 0) h = 1;

        glViewport(0, 0, w, h);
        state.camera.aspect = static_cast<float>(w) / static_cast<float>(h);

        const float yaw = state.camRot.y * DEG2RAD;
        const float pitch = state.camRot.x * DEG2RAD;
        Vec3 forward{
            std::cos(pitch) * std::sin(yaw),
            std::sin(pitch),
            std::cos(pitch) * std::cos(yaw)
        };

        Mat4 view = Mat4::lookAtLH(state.camPos, state.camPos + forward, Vec3{0.0f, 1.0f, 0.0f});
        state.renderer.render(state.scene, state.camera, view, state.camPos, w, h, 0);

        ui.setViewportSize({static_cast<float>(w), static_cast<float>(h)});
        syncInspectorFromSelection(state, ui, uiRefs);
        ui.beginFrame();
        uiRenderer.render(ui, w, h);

        SDL_GL_SwapWindow(window);
    }

    SDL_StopTextInput();
    SDL_SetRelativeMouseMode(SDL_FALSE);

    delete state.cubeMesh;
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
