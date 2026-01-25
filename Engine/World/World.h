#pragma once
#include <vector>
#include "../Graphics/Graphics.h"

struct Tile {
    int type; // 0=empty, 1=ground, 2=grass
};

class World {
public:
    World() : width(0), height(0) {}
    
    static const int TILE_SIZE = 40;
    std::vector<Tile> tiles;
    int width, height;

    void generateFlat(int w, int h, int groundY);
    void render(Graphics& g);
    float getGroundY(float x, float yBottom);

    int getTile(int tx, int ty) const;
    void setTile(int tx, int ty, int type);

    bool isSolid(int tx, int ty) const;
};
