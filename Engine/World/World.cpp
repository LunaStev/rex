#include "World.h"
#include "../Graphics/Graphics.h"

#include <algorithm>
#include <cmath>

void World::generateFlat(int w, int h, int groundTy) {
    width = std::max(0, w);
    height = std::max(0, h);

    tiles.clear();
    if (width == 0 || height == 0) return;

    tiles.resize((size_t)width * (size_t)height);

    groundTy = std::clamp(groundTy, 0, height - 1);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile& t = tiles[idx(x, y)];
            if (y > groundTy)      t.type = 1; // ground
            else if (y == groundTy) t.type = 2; // grass
            else                   t.type = 0; // empty
        }
    }
}

void World::render(Graphics& g) const {
    if (width <= 0 || height <= 0 || tiles.empty()) return;

    const auto& cam = g.getCamera();
    const float z = cam.zoom;

    // 월드에서 "화면에 보이는 영역" 계산
    const float halfW = (g.getViewportW() * 0.5f) / z;
    const float halfH = (g.getViewportH() * 0.5f) / z;

    const float worldLeft   = cam.x - halfW;
    const float worldRight  = cam.x + halfW;
    const float worldTop    = cam.y - halfH;
    const float worldBottom = cam.y + halfH;

    int tx0 = (int)std::floor(worldLeft / (float)TILE_SIZE) - 1;
    int tx1 = (int)std::floor(worldRight / (float)TILE_SIZE) + 1;
    int ty0 = (int)std::floor(worldTop / (float)TILE_SIZE) - 1;
    int ty1 = (int)std::floor(worldBottom / (float)TILE_SIZE) + 1;

    tx0 = std::clamp(tx0, 0, width - 1);
    tx1 = std::clamp(tx1, 0, width - 1);
    ty0 = std::clamp(ty0, 0, height - 1);
    ty1 = std::clamp(ty1, 0, height - 1);

    for (int ty = ty0; ty <= ty1; ++ty) {
        for (int tx = tx0; tx <= tx1; ++tx) {
            const uint8_t t = tiles[idx(tx, ty)].type;
            if (t == 0) continue;

            const float wx = (float)(tx * TILE_SIZE);
            const float wy = (float)(ty * TILE_SIZE);

            if (t == 1) {
                g.drawRectWorld(wx, wy, TILE_SIZE, TILE_SIZE, 90, 60, 30, 255);
            } else if (t == 2) {
                g.drawRectWorld(wx, wy, TILE_SIZE, TILE_SIZE, 30, 200, 60, 255);
            } else {
                // 나중에 타입 늘어나면 여기서 디폴트 컬러
                g.drawRectWorld(wx, wy, TILE_SIZE, TILE_SIZE, 200, 200, 200, 255);
            }
        }
    }
}

float World::getGroundY(float x, float yBottom) const {
    if (width <= 0 || height <= 0 || tiles.empty()) return 0.0f;

    int tx = (int)std::floor(x / (float)TILE_SIZE);
    tx = std::clamp(tx, 0, width - 1);

    int startTy = (int)std::floor(yBottom / (float)TILE_SIZE);
    startTy = std::clamp(startTy, 0, height - 1);

    for (int ty = startTy; ty < height; ++ty) {
        const uint8_t t = tiles[idx(tx, ty)].type;
        if (t != 0) return (float)(ty * TILE_SIZE);
    }

    return (float)(height * TILE_SIZE);
}

int World::getTile(int tx, int ty) const {
    // 바깥은 solid 취급 (충돌용)
    if (tx < 0 || ty < 0 || tx >= width || ty >= height) return 1;
    if (tiles.empty()) return 1;
    return (int)tiles[idx(tx, ty)].type;
}

void World::setTile(int tx, int ty, int type) {
    if (tx < 0 || ty < 0 || tx >= width || ty >= height) return;
    if (tiles.empty()) return;

    type = std::clamp(type, 0, 255);
    tiles[idx(tx, ty)].type = (uint8_t)type;
}

bool World::isSolid(int tx, int ty) const {
    const int t = getTile(tx, ty);
    return t == 1 || t == 2;
}
