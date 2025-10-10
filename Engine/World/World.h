#pragma once
#include <vector>
#include "../Graphics/Graphics.h"

struct Tile {
    int type; // 0=empty, 1=ground, 2=grass
};

class World {
public:
    static const int TILE_SIZE = 40;
    std::vector<Tile> tiles;
    int width, height;

    void generateFlat(int w, int h, int groundY);
    void render(Graphics& g);
    float getGroundY(float x, float yBottom);
};
