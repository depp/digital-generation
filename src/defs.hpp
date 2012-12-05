/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_DEFS_HPP
#define LD24_DEFS_HPP
#include "sgpp/rand.hpp"
namespace LD24 {

static const int FRAME_TIME = 8;
static const int SECOND = 1000 / FRAME_TIME;

static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;

/* Controls */
enum {
    CTL_LEFT,
    CTL_RIGHT,
    CTL_UP,
    CTL_DOWN,
    CTL_ACTION
};

enum {
    FLAG_LEFT = 1 << CTL_LEFT,
    FLAG_RIGHT = 1 << CTL_RIGHT,
    FLAG_UP = 1 << CTL_UP,
    FLAG_DOWN = 1 << CTL_DOWN,
    FLAG_ACTION = 1 << CTL_ACTION
};

inline int fix2i(int x)
{
    return (x + 128) >> 8;
}

inline int rand8()
{
    return (Rand::girand() >> 12) & 255;
}

struct Pos {
    Pos()
    { }

    Pos(int xx, int yy)
        : x(xx), y(yy)
    { }

    int x, y;

    static Pos fromDir(int dir)
    {
        switch (dir) {
        case CTL_LEFT: return Pos(-1, 0);
        case CTL_RIGHT: return Pos(1, 0);
        case CTL_UP: return Pos(0, 1);
        case CTL_DOWN: return Pos(0, -1);
        default: return Pos(0, 0);
        }
    }

    static Pos rand(int scale)
    {
        unsigned r = Rand::girand();
        int x = ((r >> 7) & 511) - 256;
        int y = ((r >> 16) & 511) - 256;
        return Pos(fix2i(x * scale), fix2i(y * scale));
    }

    Pos operator+(const Pos &o) const
    {
        return Pos(x + o.x, y + o.y);
    }

    Pos operator-(const Pos &o) const
    {
        return Pos(x - o.x, y - o.y);
    }

    Pos &operator+=(const Pos &o)
    {
        x += o.x;
        y += o.y;
        return *this;
    }

    Pos &operator-=(const Pos &o)
    {
        x -= o.x;
        y -= o.y;
        return *this;
    }

    operator bool() const
    {
        return x != 0 || y != 0;
    }
};

}
#endif
