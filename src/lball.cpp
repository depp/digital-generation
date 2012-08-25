/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "lball.hpp"
#include "sprite.hpp"
#include <cstdio>
using namespace LD24;

enum {
    S_NUMBER = 0,
    S_PADDLE = 10,
    S_DOT = 11,
    S_P = 12,
    S_WIN = 13
};

LBall::LBall(GameScreen &screen)
    : Level(screen)
{
    m_lv1 = Texture::file("img/lv1");

    m_paddlepos[0] = 0;
    m_paddlepos[1] = 0;
    m_ballx = 0;
    m_bally = 0;
    m_ballvx = 0*-256;
    m_ballvy = 0*256;

    advance(0);
    advance(0);
}

LBall::~LBall()
{ }

void LBall::advance(int controls)
{
    int pm[2];

    pm[0] = 0;
    if (controls & FLAG_UP)
        pm[0]++;
    if (controls & FLAG_DOWN)
        pm[0]--;

    pm[1] = 0;

    for (int i = 0; i < 2; ++i) {
        if (!pm[i])
            continue;
        m_paddlepos[i] += 2 * pm[i];
        if (m_paddlepos[i] < -246)
            m_paddlepos[i] = -246;
        if (m_paddlepos[i] > 246)
            m_paddlepos[i] = 246;
    }

    m_ballx += m_ballvx;
    m_bally += m_ballvy;
    m_ball.update();
    m_ball.moveTo(((m_ballx + 128) >> 8) + SCREEN_WIDTH / 2,
                  ((m_bally + 128) >> 8) + SCREEN_HEIGHT / 2);

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
}

void LBall::draw(int frac)
{
    const SpriteSheet &s = SpriteSheet::LV1;

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
        m_paddle[i].draw(s, frac, S_PADDLE);

    glColor3ub(50, 50, 50);
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < DOT_COUNT; ++j)
            m_dots[i][j].draw(s, frac, S_DOT);

    glColor3ub(255, 255, 255);
    m_ball.draw(s, frac, S_DOT);

    for (int i = 0; i < 4; ++i)
        m_score[i].draw(s, frac, S_NUMBER + i);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glColor3ub(255, 255, 255);

    (void) frac;
}
