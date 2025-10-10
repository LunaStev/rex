#include "World.h"

void World::generateFlat(int w, int h, int groundY) {
    width = w; height = h;
    tiles.resize(w * h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Tile& t = tiles[y * w + x];
            if (y > groundY) t.type = 1;
            else if (y == groundY) t.type = 2;
            else t.type = 0;
        }
    }
}

void World::render(Graphics& g) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Tile& t = tiles[y * width + x];
            if (t.type == 1)
                g.drawRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE, 90, 60, 30, 255);
            else if (t.type == 2)
                g.drawRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE, 30, 200, 60, 255);
        }
    }
}

float World::getGroundY(float x, float yBottom) {
    int tx = (int)(x / TILE_SIZE);
    for (int y = 0; y < height; y++) {
        Tile& t = tiles[y * width + tx];
        if (t.type != 0) return y * TILE_SIZE;
    }
    return height * TILE_SIZE;
}
