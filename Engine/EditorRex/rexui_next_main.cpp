#include "../Core/Logger.h"
#include "../Graphics/GLInternal.h"
#include "../UI/RexUI/App/RexUIEngine.h"
#include "../UI/RexUI/Framework/Input/InputEvent.h"
#include "../UI/RexUI/Framework/Input/InputMap.h"
#include "../UI/RexUI/Framework/Input/InputRouter.h"
#include "../UI/RexUI/Renderer/NullRenderDevice.h"
#include "../UI/RexUI/Renderer/RenderBackendFactory.h"
#include "../UI/RexUI/Renderer/RexGraphics/RexGraphicsAPI.h"
#include "../UI/RexUI/Widgets/Basic/ButtonWidget.h"
#include "../UI/RexUI/Widgets/Basic/PanelWidget.h"
#include "../UI/RexUI/Widgets/Basic/TextWidget.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <memory>
#include <string>

using namespace rex;

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Logger::error("SDL Init Error: {}", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(
        "Rex Editor Next (RexUI + RexGraphics)",
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
    ui::renderer::rexgraphics::NullRexGraphicsDevice rexGraphicsDevice;
    auto backend = ui::renderer::createRenderBackend(
        ui::renderer::RenderBackendType::RexGraphics,
        &renderDevice,
        &rexGraphicsDevice);
    if (!backend) {
        Logger::error("Failed to create RexGraphics backend");
        SDL_GL_DeleteContext(gl);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    ui::app::RexUIEngine uiEngine(backend.get());
    ui::framework::input::InputMap inputMap;
    ui::framework::input::InputRouter inputRouter(&inputMap);

    auto root = std::make_shared<ui::widgets::basic::PanelWidget>();
    root->setStyleClass("editor.root");
    root->setOrientation(ui::widgets::basic::PanelOrientation::Vertical);

    auto title = std::make_shared<ui::widgets::basic::TextWidget>();
    title->setText("RexUI Next: no legacy dependency path");
    title->setStyleClass("editor.title");
    root->addChild(title);

    auto status = std::make_shared<ui::widgets::basic::TextWidget>();
    status->setText("Commands submitted: 0");
    status->setStyleClass("editor.status");
    root->addChild(status);

    auto clickButton = std::make_shared<ui::widgets::basic::ButtonWidget>();
    clickButton->setText("Generate UI Work");
    clickButton->setStyleClass("editor.button");
    std::uint64_t clickCount = 0;
    clickButton->setOnClick([&]() {
        ++clickCount;
        status->setText("Commands submitted: " + std::to_string(rexGraphicsDevice.submittedCommandCount()) +
            " | Clicks: " + std::to_string(clickCount));
    });
    root->addChild(clickButton);

    uiEngine.setRoot(root);
    uiEngine.stateStore().set("editor.mode", std::string("RexGraphics"));

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
        uiEngine.runFrame(dt, frameIndex++);

        SDL_GL_SwapWindow(window);
    }

    SDL_StopTextInput();
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

