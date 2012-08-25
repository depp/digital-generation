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
        : px(0), py(0), ppx(0), ppy(0)
    { }

    void update()
    {
        ppx = px;
        ppy = py;
    }

    void moveTo(int x, int y)
    {
        px = x;
        py = y;
    }

    void setPos(int x, int y)
    {
        ppx = px = x;
        ppy = py = y;
    }

    void draw(const SpriteSheet &sp, int frac,
              int n, float sx, float sy) const
    {
        int x = ppx + (px - ppx) * frac / FRAME_TIME;
        int y = ppy + (py - ppy) * frac / FRAME_TIME;
        sp.draw(n, x, y, sx, sy);
    }

    void draw(const SpriteSheet &sp, int frac, int n, float s) const
    {
        draw(sp, frac, n, s, s);
    }

    void draw(const SpriteSheet &sp, int frac, int n) const
    {
        draw(sp, frac, n, 1.0f, 1.0f);
    }

    short px, py;
    short ppx, ppy;
};


}
#endif
