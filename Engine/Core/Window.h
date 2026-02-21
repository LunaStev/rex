#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace rex {

struct WindowConfig {
    std::string title = "Rex Engine";
    int width = 1280;
    int height = 720;
    bool vsync = true;
};

class Window {
public:
    Window(const WindowConfig& config);
    ~Window();

    bool shouldClose() const { return m_shouldClose; }
    void pollEvents();
    void swapBuffers();
    
    void setVSync(bool enabled);
    
    SDL_Window* getNativeWindow() const { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    int m_width, m_height;
    bool m_shouldClose = false;
};

}
