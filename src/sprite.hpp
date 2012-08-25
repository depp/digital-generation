/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_SPRITE_HPP
#define LD24_SPRITE_HPP
namespace LD24 {

class Sprites {
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
    Sprites(int w, int h, const Sprite *s, int n)
        : width(w), height(h), sprites(s), count(n)
    { }

    void draw(int n, float x, float y, float scale) const;

    static const Sprites CHARGE;
    static const Sprites LV1;
};

}
#endif
