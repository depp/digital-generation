/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_LEVEL_HPP
#define LD24_LEVEL_HPP
#include <vector>
#include <stddef.h>
namespace LD24 {
class GameScreen;

class Level {
    const char *const *m_allTips;
    unsigned m_tipflag;

    void setTipFlags(unsigned flags);

public:
    GameScreen &screen;
    std::vector<const char *> levelTips;

    Level(GameScreen &s, const char *const *tips=NULL)
        : m_allTips(tips), m_tipflag(0), screen(s)
    { }

    virtual ~Level();

    /* Advance one frame, 8 milliseconds.  The controls are the FLAG
       constant above, combined to show which buttons the user has
       down.  */
    virtual void advance(unsigned msec, int controls) = 0;

    /* Draw the frame (frac is milliseconds since 'advance') */
    virtual void draw(int frac) = 0;

    void nextLevel();

    void addTip(int index);
    void removeTip(int index);
};

}
#endif
