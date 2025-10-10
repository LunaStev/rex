#pragma once
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include "../Graphics/Graphics.h"

class Text {
private:
    TTF_Font* font = nullptr;

public:
    Text() = default;
    ~Text() { quit(); }

    bool init(const std::string& fontPath, int fontSize);
    void render(Graphics& g, const std::string& text, int x, int y,
                SDL_Color color = {255, 255, 255, 255});
    void quit();
};
