/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_SPRITE_HPP
#define LD24_SPRITE_HPP
#include "defs.hpp"
namespace LD24 {

class SpriteSheet {
public:
    struct Sprite {
        short x0, y0, x1, y1, xc, yc;
    };

private:
    int width;
    int height;
    const Sprite *sprites;
    int count;

public:
    SpriteSheet(int w, int h, const Sprite *s, int n)
        : width(w), height(h), sprites(s), count(n)
    { }

    void draw(int n, float x, float y, float sx, float sy) const;

    void draw(int n, float x, float y, float s) const
    {
        draw(n, x, y, s, s);
    }

    void draw(int n, float x, float y) const
    {
        draw(n, x, y, 1.0f, 1.0f);
    }

    static const SpriteSheet CHARGE;
    static const SpriteSheet LV1;
};

struct Sprite {
    Sprite()
        : x(0), y(0), px(0), py(0)
    { }

    void update()
    {
        px = x;
        py = y;
    }

    void moveTo(int xx, int yy)
    {
        x = xx;
        y = yy;
    }

    void setPos(int xx, int yy)
    {
        px = x = xx;
        py = y = yy;
    }

    void draw(const SpriteSheet &sp, int frac,
              int n, float sxx, float syy) const
    {
        int xx = px + (x - px) * frac / FRAME_TIME;
        int yy = py + (y - py) * frac / FRAME_TIME;
        sp.draw(n, xx, yy, sxx, syy);
    }

    void draw(const SpriteSheet &sp, int frac, int n, float s) const
    {
        draw(sp, frac, n, s, s);
    }

    void draw(const SpriteSheet &sp, int frac, int n) const
    {
        draw(sp, frac, n, 1.0f, 1.0f);
    }

    void getPos(int frac, int &xx, int &yy) const
    {
        xx = px + (x - px) * frac / FRAME_TIME;
        yy = py + (y - py) * frac / FRAME_TIME;
    }

    short x, y;
    short px, py;
};

namespace LV2 {

extern const char TEXNAME[];
extern const SpriteSheet SPRITES;

enum {
    DOOR,
    KEY,
    PLAT,
    LADDER,
    PLAYER,
    PLAYERX,
    MON1,
    MON1X,
    MON2,
    MON2X,
    MON3,
    MON3X,

    COUNT
};

}

namespace LV3 {

extern const char TEXNAME[];
extern const SpriteSheet SPRITES;

enum {
    BASE,
    BUNK1,
    BUNK2,
    PLYR1,
    PLYR2,
    PLYL1,
    PLYL2,
    TANK,
    SHOT1L,
    SHOT1C,
    SHOT1R,
    POOF1,
    SHOT2,
    POOF2,
    SHIP1,
    SHIP2,
    SHIP3,
    KITR,
    KITL,

    COUNT
};

}

}
#endif
