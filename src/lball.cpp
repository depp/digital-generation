/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "lball.hpp"
#include "sprite.hpp"
#include "client/rand.hpp"
#include <cstdio>
#include <cstring>
using namespace LD24;

enum {
    SP_NUMBER = 0,
    SP_PADDLE = 10,
    SP_DOT = 11,
    SP_P = 12,
    SP_WIN = 13
};

LBall::LBall(GameScreen &screen)
    : Level(screen)
{
    m_lv1 = Texture::file("img/lv1");

    m_state = ST_START;
    m_tick = 0;
    m_round = 0;

    m_paddlepos[0] = 0;
    m_paddlepos[1] = 0;
    m_ballx = 0;
    m_bally = 0;
    m_ballvx = 0;
    m_ballvy = 0;

    m_npoints[0] = 99;
    m_npoints[1] = 0;

    advance(0, 0);
    advance(0, 0);
    m_state = ST_START;

    m_agame.open();

    static const char FX_NAME[FX_COUNT][5] = {
        "png", "bad", "good", "jmp", "land", "step", "boom"
    };

    for (int i = 0; i < FX_COUNT; ++i) {
        char name[16];
        std::strcpy(name, "fx/1_");
        std::strcat(name, FX_NAME[i]);
        m_fx[i] = AudioFile::file(name);
    }
}

LBall::~LBall()
{ }

void LBall::advance(unsigned msec, int controls)
{
    int pm[2];
    State st = m_state;

    m_tick += 1;
    switch (st) {
    case ST_INIT:
        break;

    case ST_START:
        if (m_tick > 1 * SECOND) {
            m_state = ST_PLAY;
            m_ballvx = 512;
            m_ballvy = (int) (Rand::girand() & 1023) - 512;
            if (m_round & 1)
                m_ballvx = -m_ballvx;
        }
        break;

    case ST_PLAY:
        break;

    case ST_POINT_P1:
    case ST_POINT_P2:
        if (m_tick > 2 * SECOND)
            goto start;
        break;

    case ST_MATCH_P1:
    case ST_MATCH_P2:
        if (m_tick > 8 * SECOND)
            goto start;
        break;

    start:
        m_ballx = 0;
        m_bally = 0;
        m_state = ST_START;
    }

    pm[0] = 0;
    if (controls & FLAG_UP)
        pm[0]++;
    if (controls & FLAG_DOWN)
        pm[0]--;

    pm[1] = 0;
    if (st == ST_PLAY) {
        int d = fix2i(m_bally) - m_paddlepos[1];
        if (d > 16)
            pm[1] = 1;
        else if (d < -16)
            pm[1] = -1;
    }

    for (int i = 0; i < 2; ++i) {
        if (!pm[i])
            continue;
        m_paddlepos[i] += 2 * pm[i];
        if (m_paddlepos[i] < -PADDLE_YMAX)
            m_paddlepos[i] = -PADDLE_YMAX;
        if (m_paddlepos[i] > PADDLE_YMAX)
            m_paddlepos[i] = PADDLE_YMAX;
    }

    m_ball.update();
    if (st == ST_PLAY) {
        bool bounce = false;

        m_bally += m_ballvy;
        if (m_bally >= BALL_YMAX * 256) {
            bounce = true;
            m_bally = BALL_YMAX * 256;
            m_ballvy = -m_ballvy;
        } else if (m_bally <= -BALL_YMAX * 256) {
            bounce = true;
            m_bally = -BALL_YMAX * 256;
            m_ballvy = -m_ballvy;
        }

        m_ballx += m_ballvx;
        int paddle = -1;
        if (m_ballx >= BALL_XMAX * 256) {
            paddle = 1;
            m_ballx = BALL_XMAX * 256;
        } else if (m_ballx <= -BALL_XMAX * 256) {
            paddle = 0;
            m_ballx = -BALL_XMAX * 256;
        }
        if (paddle >= 0) {
            int delta = fix2i(m_bally) - m_paddlepos[paddle];
            if (delta <= CONTACT_HEIGHT/2 &&
                delta >= -CONTACT_HEIGHT/2)
            {
                bounce = true;
                m_ballvx = -m_ballvx;
                m_ballvy = (delta * 512) / (CONTACT_HEIGHT / 2);
            } else {
                bounce = false;
                if (paddle == 0) {
                    m_state = ST_POINT_P2;
                    m_agame.play(msec, *m_fx[FX_BAD], 0);
                    m_npoints[1] += 1;
                } else {
                    m_state = ST_POINT_P1;
                    m_agame.play(msec, *m_fx[FX_GOOD], 0);
                    m_npoints[0] += 1;
                }
            }
        }

        if (bounce)
            m_agame.play(msec, *m_fx[FX_PNG], 0);
    }
    m_ball.moveTo(fix2i(m_ballx) + SCREEN_WIDTH / 2,
                  fix2i(m_bally) + SCREEN_HEIGHT / 2);
    if (st == ST_START)
        m_ball.update();

    for (int i = 0; i < 2; ++i) {
        m_paddle[i].update();
        m_paddle[i].moveTo(!i ? 32 : SCREEN_WIDTH - 32,
                           SCREEN_HEIGHT / 2 + m_paddlepos[i]);
    }

    for (int i = 0; i < DOT_COUNT; ++i) {
        m_dots[0][i].update();
        m_dots[0][i].moveTo(dotPos(i), DOT_LHEIGHT);
    }
    for (int i = 0; i < DOT_COUNT; ++i) {
        m_dots[1][i].update();
        m_dots[1][i].moveTo(dotPos(i), SCREEN_HEIGHT - DOT_LHEIGHT);
    }

    for (int i = 0; i < 4; ++i) {
        int delta = i < 2 ? -64 : 64;
        m_score[i].update();
        m_score[i].moveTo(SCREEN_WIDTH / 2 - 60 + delta + i * 40,
                          SCREEN_HEIGHT - 32 - 96);
    }

    if (m_state != st)
        m_tick = 0;
}

static void pscore(unsigned char *p, unsigned x)
{
    p[1] = x % 10;
    x /= 10;
    p[0] = x % 10;
}

void LBall::draw(int frac)
{
    const SpriteSheet &s = SpriteSheet::LV1;
    State st = m_state;

    //glClearColor(0.1, 0.1, 0.1, 0.0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    glBegin(GL_TRIANGLES);

/*
    for (int i = 0; i < 10; ++i)
        s.draw(i, 64 + 40*i, 64, 1.0f);
    s.draw(10, 64, 256, 1.0f);
    s.draw(11, 128, 256, 1.0f);
    s.draw(12, 64, 384, 1.0f);
    s.draw(13, 128, 384, 1.0f);
*/

    glColor3ub(255, 255, 255);
    for (int i = 0; i < 2; ++i)
        m_paddle[i].draw(s, frac, SP_PADDLE);

    glColor3ub(50, 50, 50);
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < DOT_COUNT; ++j)
            m_dots[i][j].draw(s, frac, SP_DOT);

    glColor3ub(255, 255, 255);
    if (st == ST_PLAY || st == ST_START)
        m_ball.draw(s, frac, SP_DOT);

    unsigned char pts[4];
    pscore(pts + 0, m_npoints[0]);
    pscore(pts + 2, m_npoints[1]);
    for (int i = 0; i < 4; ++i)
        m_score[i].draw(s, frac, SP_NUMBER + pts[i]);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glColor3ub(255, 255, 255);

    (void) frac;
}
