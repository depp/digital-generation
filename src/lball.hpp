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

    static const int AREA_YMAX = 37 * 8;

    static const int PADDLE_HEIGHT = 12 * 8;
    static const int BALL_HEIGHT = 2 * 8;
    static const int CONTACT_HEIGHT = PADDLE_HEIGHT + BALL_HEIGHT;
    static const int BALL_XMAX = SCREEN_WIDTH / 2 - 48;
    static const int BALL_YMAX = AREA_YMAX - BALL_HEIGHT/2;
    static const int PADDLE_YMAX = AREA_YMAX - PADDLE_HEIGHT / 2;

    typedef enum {
        ST_START,
        ST_PLAY,
        ST_POINT_P1,
        ST_POINT_P2,
        ST_MATCH_P1,
        ST_MATCH_P2
    } State;

    State m_state;
    int m_tick;
    int m_round;

    int m_paddlepos[2];
    Sprite m_paddle[2];
    Sprite m_dots[2][DOT_COUNT];

    int m_ballx, m_bally;
    int m_ballvx, m_ballvy;
    Sprite m_ball;

    int m_npoints[2];
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

    virtual void advance(unsigned msec, int controls);

    virtual void draw(int frac);
};

}
#endif
