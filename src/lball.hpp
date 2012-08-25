/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_LBALL_HPP
#define LD24_LBALL_HPP
#include "defs.hpp"
#include "level.hpp"
#include "sprite.hpp"
#include "client/audio.hpp"
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

    static const int SPEED_MIN = 512;
    static const int SPEED_MAX = SPEED_MIN * 3;
    static const int SPEED_STEP = SPEED_MIN / 4;

    static const int PLAYER_XSPEED = 512 * 4,
        PLAYER_GRAVITY = 48,
        PLAYER_MAXFALL = 1024,
        PLAYER_JUMP = 512 * 6;

    typedef enum {
        ST_INIT,
        ST_START,
        ST_PLAY,
        ST_POINT_P1,
        ST_POINT_P2,
        ST_MATCH_P1,
        ST_MATCH_P2
    } State;

    typedef enum {
        SS_BEGIN,
        SS_CHAOS,
        SS_SPIT,
        SS_OUT,
        SS_IN
    } SState;

    typedef enum {
        FX_PNG,
        FX_BAD,
        FX_GOOD,
        FX_JMP,
        FX_LAND,
        FX_STEP,
        FX_BOOM
    } Effect;

    static const int FX_COUNT = (int) FX_BOOM + 1;

    unsigned m_lastctl;

    State m_state;
    int m_tick;
    int m_round;
    int m_bounce;

    SState m_sstate;
    int m_stick;

    int m_paddlepos[2];
    Sprite m_paddle[2];
    Sprite m_dots[2][DOT_COUNT];

    int m_ballx, m_bally;
    int m_ballvx, m_ballvy;
    Sprite m_ball;

    int m_npoints[2];
    Sprite m_score[4];

    int m_playx, m_playy;
    int m_playvx, m_playvy;
    Sprite m_player;

    Texture::Ref m_lv1;

    /* SOUND */
    AudioSource m_agame;
    AudioSource m_aplayer;
    AudioFile::Ref m_fx[FX_COUNT];
    int m_sball;

    static int dotPos(int i)
    {
        return (SCREEN_WIDTH - (DOT_COUNT - 1) * DOT_LSPACING) / 2 +
            i * DOT_LSPACING;
    }

public:
    LBall(GameScreen &screen);
    virtual ~LBall();

    virtual void advance(unsigned time, int controls);

    virtual void draw(int frac);
};

}
#endif
