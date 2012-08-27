/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_LEVEL_HPP
#define LD24_LEVEL_HPP
#include <vector>
namespace LD24 {
class GameScreen;

class Level {
public:
    GameScreen &screen;
    std::vector<const char *> levelTips;

    Level(GameScreen &s)
        : screen(s)
    { }

    virtual ~Level();

    /* Advance one frame, 8 milliseconds.  The controls are the FLAG
       constant above, combined to show which buttons the user has
       down.  */
    virtual void advance(unsigned msec, int controls) = 0;

    /* Draw the frame (frac is milliseconds since 'advance') */
    virtual void draw(int frac) = 0;

    void nextLevel();
};

}
#endif
