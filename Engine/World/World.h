#pragma once
#include <vector>
#include <cstdint>

class Graphics;

struct Tile {
    uint8_t type = 0; // 0=empty, 1=ground, 2=grass
};

class World {
public:
    static constexpr int TILE_SIZE = 40;

    int width = 0;
    int height = 0;
    std::vector<Tile> tiles;

    void generateFlat(int w, int h, int groundTy);

    // 카메라/줌 적용해서 렌더
    void render(Graphics& g) const;

    // x 컬럼에서 yBottom(픽셀) 기준 아래쪽으로 첫 solid 타일의 top Y(픽셀) 반환
    float getGroundY(float x, float yBottom) const;

    int  getTile(int tx, int ty) const;
    void setTile(int tx, int ty, int type);

    bool isSolid(int tx, int ty) const;

private:
    inline int idx(int tx, int ty) const { return ty * width + tx; }
};
