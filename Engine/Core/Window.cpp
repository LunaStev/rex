#include "Window.h"
#include "Logger.h"
#include "../Graphics/GLInternal.h"

namespace rex {

Window::Window(const WindowConfig& config) 
    : m_width(config.width), m_height(config.height) {
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Logger::error("SDL Init Error: {}", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_window = SDL_CreateWindow(
        config.title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
    );

    if (!m_window) {
        Logger::error("Window Error: {}", SDL_GetError());
        return;
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        Logger::error("GL Context Error: {}", SDL_GetError());
        return;
    }

    loadGLFunctionsSDL();

    setVSync(config.vsync);
    Logger::info("Rex SDL Window initialized: {}x{}", m_width, m_height);
}

Window::~Window() {
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::pollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) m_shouldClose = true;
    }
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(m_window);
}

void Window::setVSync(bool enabled) {
    SDL_GL_SetSwapInterval(enabled ? 1 : 0);
}

}
