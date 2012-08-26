/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_LTITLE_HPP
#define LD24_LTITLE_HPP
#include "defs.hpp"
#include "level.hpp"
namespace LD24 {

class LTitle : public Level {
private:
    static const int INTER_TIME = SECOND;

    const char *m_title;
    bool m_is_end;
    int m_time;
    int m_width;

public:
    LTitle(GameScreen &screen,
           const char *title, bool is_end);
    virtual ~LTitle();

    virtual void advance(unsigned time, int controls);

    virtual void draw(int frac);
};

}
#endif
