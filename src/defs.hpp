/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_DEFS_HPP
#define LD24_DEFS_HPP
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

}
#endif
