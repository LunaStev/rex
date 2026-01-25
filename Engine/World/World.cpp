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
    (void)yBottom;

    int tx = (int)(x / TILE_SIZE);
    if (tx < 0) tx = 0;
    if (tx >= width) tx = width - 1;

    for (int y = 0; y < height; y++) {
        Tile& t = tiles[y * width + tx];
        if (t.type != 0) return y * TILE_SIZE;
    }
    return height * TILE_SIZE;
}

int World::getTile(int tx, int ty) const {
    if (tx < 0 || ty < 0 || tx >= width || ty >= height) return 1;
    return tiles[ty * width + tx].type;
}

void World::setTile(int tx, int ty, int type) {
    if (tx < 0 || ty < 0 || tx >= width || ty >= height) return;
    tiles[ty * width + tx].type = type;
}

bool World::isSolid(int tx, int ty) const {
    return getTile(tx, ty) != 0;
}

