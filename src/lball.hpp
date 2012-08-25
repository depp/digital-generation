/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_LBALL_HPP
#define LD24_LBALL_HPP
#include "defs.hpp"
#include "level.hpp"
#include "sprite.hpp"
#include "client/texture.hpp"
namespace LD24 {

class LBall : public Level {
private:
    static const int DOT_COUNT = 39;
    static const int DOT_LSPACING = 32;
    static const int DOT_LHEIGHT = 48;

    int m_paddlepos[2];
    Sprite m_paddle[2];
    Sprite m_dots[2][DOT_COUNT];

    int m_ballx, m_bally;
    int m_ballvx, m_ballvy;
    Sprite m_ball;

    Sprite m_score[4];

    Texture::Ref m_lv1;

    static int dotPos(int i)
    {
        return (SCREEN_WIDTH - (DOT_COUNT - 1) * DOT_LSPACING) / 2 +
            i * DOT_LSPACING;
    }

public:
    LBall(GameScreen &screen);
    virtual ~LBall();

    virtual void advance(int controls);

    virtual void draw(int frac);
};

}
#endif
