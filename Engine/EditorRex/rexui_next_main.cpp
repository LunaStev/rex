#include "../Core/Logger.h"
#include "../Editor/Core/EditorApp.h"
#include "../Graphics/GLInternal.h"
#include "../UI/RexUI/App/RexUIEngine.h"
#include "../UI/RexUI/Core/PaintContext.h"
#include "../UI/RexUI/Core/Widget.h"
#include "../UI/RexUI/Framework/Input/InputEvent.h"
#include "../UI/RexUI/Framework/Input/InputMap.h"
#include "../UI/RexUI/Framework/Input/InputRouter.h"
#include "../UI/RexUI/Renderer/NullRenderDevice.h"
#include "../UI/RexUI/Renderer/RenderBackendFactory.h"
#include "../UI/RexUI/Widgets/Basic/ButtonWidget.h"
#include "../UI/RexUI/Widgets/Basic/PanelWidget.h"
#include "../UI/RexUI/Widgets/Basic/TextWidget.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <variant>

using namespace rex;

namespace {

class EditorShellWidget final : public ui::core::Widget {
public:
    ui::core::Size measure(const ui::core::LayoutConstraints& constraints, const ui::core::EventContext& ctx) const override {
        (void)ctx;
        ui::core::Size out = constraints.max;
        if (out.w <= 0.0f) out.w = 1280.0f;
        if (out.h <= 0.0f) out.h = 720.0f;
        if (out.w < constraints.min.w) out.w = constraints.min.w;
        if (out.h < constraints.min.h) out.h = constraints.min.h;
        return out;
    }

    void arrange(const ui::core::Rect& finalRect) override {
        ui::core::Widget::arrange(finalRect);
        const auto& items = children();
        if (items.size() < 7) return;

        const float menuHeight = 30.0f;
        const float toolbarHeight = 30.0f;
        const float statusHeight = 24.0f;
        const float bottomHeight = std::max(170.0f, finalRect.h * 0.24f);

        float leftWidth = std::max(220.0f, finalRect.w * 0.18f);
        float rightWidth = std::max(280.0f, finalRect.w * 0.22f);
        constexpr float minCenterWidth = 320.0f;
        const float maxSideTotal = std::max(0.0f, finalRect.w - minCenterWidth);
        if (leftWidth + rightWidth > maxSideTotal) {
            const float scale = maxSideTotal > 0.0f ? (maxSideTotal / (leftWidth + rightWidth)) : 0.0f;
            leftWidth *= scale;
            rightWidth *= scale;
        }

        const float topY = finalRect.y;
        const float menuY = topY;
        const float toolbarY = menuY + menuHeight;
        const float centerY = toolbarY + toolbarHeight;
        const float centerHeight = std::max(100.0f, finalRect.h - (menuHeight + toolbarHeight + bottomHeight + statusHeight));
        const float bottomY = centerY + centerHeight;
        const float statusY = bottomY + bottomHeight;

        const float centerWidth = std::max(minCenterWidth, finalRect.w - leftWidth - rightWidth);
        const float rightX = finalRect.x + finalRect.w - rightWidth;
        const float centerX = finalRect.x + leftWidth;

        items[0]->arrange({finalRect.x, menuY, finalRect.w, menuHeight});                    // menu
        items[1]->arrange({finalRect.x, toolbarY, finalRect.w, toolbarHeight});              // toolbar
        items[2]->arrange({finalRect.x, centerY, leftWidth, centerHeight});                   // outliner
        items[3]->arrange({centerX, centerY, centerWidth, centerHeight});                     // viewport
        items[4]->arrange({rightX, centerY, rightWidth, centerHeight});                       // details
        items[5]->arrange({finalRect.x, bottomY, finalRect.w, bottomHeight});                 // bottom
        items[6]->arrange({finalRect.x, statusY, finalRect.w, statusHeight});                 // status
    }

    void paint(ui::core::PaintContext& ctx) const override {
        ctx.drawRect(arrangedRect(), ui::core::Color{0.04f, 0.06f, 0.10f, 1.0f});
        ui::core::Widget::paint(ctx);
    }
};

std::shared_ptr<ui::widgets::basic::PanelWidget> makePanel(const ui::core::Color& bg,
                                                           const ui::core::Color& border,
                                                           float borderThickness = 1.0f) {
    auto panel = std::make_shared<ui::widgets::basic::PanelWidget>();
    panel->setBackgroundColor(bg);
    panel->setBorderColor(border);
    panel->setBorderThickness(borderThickness);
    return panel;
}

std::shared_ptr<ui::widgets::basic::TextWidget> makeText(const std::string& text) {
    auto w = std::make_shared<ui::widgets::basic::TextWidget>();
    w->setText(text);
    return w;
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
        "Rex Editor Next (RexUI OpenGL Preview)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
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
    SDL_StartTextInput();

    ui::renderer::NullRenderDevice renderDevice;
    auto backend = ui::renderer::createRenderBackend(
        ui::renderer::RenderBackendType::OpenGL,
        &renderDevice);
    if (!backend) {
        Logger::error("Failed to create OpenGL RexUI backend");
        SDL_GL_DeleteContext(gl);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    ui::app::RexUIEngine uiEngine(backend.get());
    ui::framework::input::InputMap inputMap;
    ui::framework::input::InputRouter inputRouter(&inputMap);

    auto root = std::make_shared<EditorShellWidget>();
    root->setStyleClass("editor.root");

    auto menuBar = makePanel({0.13f, 0.15f, 0.18f, 1.0f}, {0.20f, 0.24f, 0.30f, 1.0f});
    menuBar->setOrientation(ui::widgets::basic::PanelOrientation::Horizontal);
    menuBar->addChild(makeText("File"));
    menuBar->addChild(makeText("Edit"));
    menuBar->addChild(makeText("View"));
    menuBar->addChild(makeText("Build"));
    menuBar->addChild(makeText("Settings"));
    menuBar->addChild(makeText("Help"));

    auto toolbar = makePanel({0.10f, 0.12f, 0.16f, 1.0f}, {0.18f, 0.22f, 0.28f, 1.0f});
    toolbar->setOrientation(ui::widgets::basic::PanelOrientation::Horizontal);
    toolbar->addChild(makeText("Select"));
    toolbar->addChild(makeText("Move"));
    toolbar->addChild(makeText("Rotate"));
    toolbar->addChild(makeText("Scale"));
    toolbar->addChild(makeText("Play"));
    toolbar->addChild(makeText("Pause"));

    auto outlinerPanel = makePanel({0.09f, 0.11f, 0.14f, 1.0f}, {0.18f, 0.22f, 0.28f, 1.0f});
    auto outlinerTitle = makeText("World Outliner");
    auto outlinerInfo = makeText("Selected entities: 0");
    outlinerPanel->addChild(outlinerTitle);
    outlinerPanel->addChild(outlinerInfo);

    auto viewportPanel = makePanel({0.06f, 0.08f, 0.12f, 1.0f}, {0.20f, 0.28f, 0.36f, 1.0f});
    auto viewportTitle = makeText("Scene Viewport");
    auto viewportInfo = makeText("Mode: Lit");
    viewportPanel->addChild(viewportTitle);
    viewportPanel->addChild(viewportInfo);

    auto detailsPanel = makePanel({0.09f, 0.10f, 0.13f, 1.0f}, {0.18f, 0.22f, 0.28f, 1.0f});
    auto detailsTitle = makeText("Details");
    auto detailsInfo = makeText("No selection");
    detailsPanel->addChild(detailsTitle);
    detailsPanel->addChild(detailsInfo);

    auto bottomArea = makePanel({0.08f, 0.10f, 0.12f, 1.0f}, {0.18f, 0.22f, 0.28f, 1.0f});
    bottomArea->setOrientation(ui::widgets::basic::PanelOrientation::Horizontal);
    auto contentPanel = makePanel({0.08f, 0.10f, 0.12f, 1.0f}, {0.16f, 0.20f, 0.26f, 1.0f});
    auto outputPanel = makePanel({0.08f, 0.10f, 0.12f, 1.0f}, {0.16f, 0.20f, 0.26f, 1.0f});
    auto contentTitle = makeText("Content Browser");
    auto contentInfo = makeText("Assets: 0 | Filter: *");
    auto outputTitle = makeText("Output Log");
    auto outputInfo = makeText("Editor started");
    contentPanel->addChild(contentTitle);
    contentPanel->addChild(contentInfo);
    outputPanel->addChild(outputTitle);
    outputPanel->addChild(outputInfo);
    bottomArea->addChild(contentPanel);
    bottomArea->addChild(outputPanel);

    auto statusPanel = makePanel({0.12f, 0.14f, 0.17f, 1.0f}, {0.20f, 0.24f, 0.30f, 1.0f});
    auto status = makeText("Ready");
    statusPanel->addChild(status);

    root->addChild(menuBar);
    root->addChild(toolbar);
    root->addChild(outlinerPanel);
    root->addChild(viewportPanel);
    root->addChild(detailsPanel);
    root->addChild(bottomArea);
    root->addChild(statusPanel);

    uiEngine.setRoot(root);
    uiEngine.stateStore().set("editor.mode", std::string("Editor"));

    editor::core::EditorApp editorApp(&uiEngine);
    editor::core::EditorWorkspace workspace{};
    workspace.projectRoot = ".";
    workspace.contentRoot = "./Assets";
    workspace.configRoot = "./Config";
    workspace.cacheRoot = "./Cache";
    if (!editorApp.initialize(workspace)) {
        Logger::error("Failed to initialize EditorApp");
        SDL_StopTextInput();
        SDL_GL_DeleteContext(gl);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    editorApp.stateStore().state().activeScenePath = "Scenes/Sandbox.rexscene";
    editorApp.stateStore().rawStore().set("editor.scene.path", std::string("Scenes/Sandbox.rexscene"));

    bool running = true;
    std::uint64_t frameIndex = 0;
    std::uint64_t lastTicks = SDL_GetTicks64();

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = false;
                break;
            }

            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
                break;
            }
            if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_1) editorApp.selection().selectEntity(1, false);
                if (ev.key.keysym.sym == SDLK_2) editorApp.selection().selectEntity(2, true);
                if (ev.key.keysym.sym == SDLK_3) editorApp.selection().selectEntity(3, true);
                if (ev.key.keysym.sym == SDLK_4) editorApp.selection().clear();
            }

            if (ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP) {
                ui::framework::input::InputEvent input{};
                input.device = ui::framework::input::InputDeviceType::Mouse;
                input.type = (ev.type == SDL_MOUSEBUTTONDOWN)
                    ? ui::framework::input::InputEventType::ButtonDown
                    : ui::framework::input::InputEventType::ButtonUp;
                input.code = ev.button.button;
                inputRouter.routeToWidgetTree(root.get(), input);
            }

            if (ev.type == SDL_TEXTINPUT) {
                ui::framework::input::InputEvent input{};
                input.device = ui::framework::input::InputDeviceType::Keyboard;
                input.type = ui::framework::input::InputEventType::Text;
                input.text = ev.text.text;
                inputRouter.routeToWidgetTree(root.get(), input);
            }
        }

        const std::uint64_t now = SDL_GetTicks64();
        float dt = static_cast<float>(now - lastTicks) / 1000.0f;
        lastTicks = now;
        dt = std::clamp(dt, 0.0f, 0.1f);

        int w = 0;
        int h = 0;
        SDL_GetWindowSize(window, &w, &h);
        if (w <= 0) w = 1;
        if (h <= 0) h = 1;

        glViewport(0, 0, w, h);
        glClearColor(0.06f, 0.07f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        uiEngine.setViewport(static_cast<std::uint32_t>(w), static_cast<std::uint32_t>(h));
        if (!editorApp.tick(dt, frameIndex++)) {
            Logger::error("Editor frame failed");
            running = false;
            continue;
        }

        std::int64_t panelCount = 0;
        if (const auto v = editorApp.stateStore().rawStore().get("editor.panels.active_count")) {
            if (const auto* n = std::get_if<std::int64_t>(&*v)) {
                panelCount = *n;
            }
        }
        std::int64_t selectedEntityCount = 0;
        if (const auto v = editorApp.stateStore().rawStore().get("editor.selection.entity_count")) {
            if (const auto* n = std::get_if<std::int64_t>(&*v)) {
                selectedEntityCount = *n;
            }
        }
        bool canUndo = false;
        if (const auto v = editorApp.stateStore().rawStore().get("editor.commands.can_undo")) {
            if (const auto* b = std::get_if<bool>(&*v)) {
                canUndo = *b;
            }
        }
        bool canRedo = false;
        if (const auto v = editorApp.stateStore().rawStore().get("editor.commands.can_redo")) {
            if (const auto* b = std::get_if<bool>(&*v)) {
                canRedo = *b;
            }
        }

        outlinerInfo->setText("Selected entities: " + std::to_string(selectedEntityCount));
        detailsInfo->setText(
            selectedEntityCount > 0
                ? "Selection count: " + std::to_string(selectedEntityCount)
                : "No selection");
        viewportInfo->setText("Scene: " + editorApp.stateStore().state().activeScenePath);
        contentInfo->setText("Assets: 0 | Filter: *");
        outputInfo->setText("Panels: " + std::to_string(panelCount) + " | Editor tick running");
        status->setText(
            "Panels " + std::to_string(panelCount) +
            " | Selected " + std::to_string(selectedEntityCount) +
            " | Undo " + (canUndo ? "Yes" : "No") +
            " | Redo " + (canRedo ? "Yes" : "No"));

        SDL_GL_SwapWindow(window);
    }

    editorApp.shutdown();
    backend.reset();
    SDL_StopTextInput();
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
