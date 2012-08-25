/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_LBALL_HPP
#define LD24_LBALL_HPP
#include "level.hpp"
namespace LD24 {

class LBall {
public:
    LBall();
    virtual ~LBall();

    virtual void advance(int controls);

    virtual void draw(unsigned frac);
}

}
#endif
