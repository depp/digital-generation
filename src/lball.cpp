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
    int i;

    m_lv1 = Texture::file("img/lv1");

    for (i = 0; i < 2; ++i)
        m_paddlepos[i] = 0;
    for (i = 0; i < DOT_COUNT; ++i)
        m_dots[0][i].setPos(dotPos(i), DOT_LHEIGHT);
    for (i = 0; i < DOT_COUNT; ++i)
        m_dots[1][i].setPos(dotPos(i), SCREEN_HEIGHT - DOT_LHEIGHT);
}

LBall::~LBall()
{ }

void LBall::advance(int controls)
{
    (void) controls;
}

void LBall::draw(int frac)
{
    const SpriteSheet &s = SpriteSheet::LV1;

    //glClearColor(0.1, 0.1, 0.1, 0.0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);

/*
    for (int i = 0; i < 10; ++i)
        s.draw(i, 64 + 40*i, 64, 1.0f);
    s.draw(10, 64, 256, 1.0f);
    s.draw(11, 128, 256, 1.0f);
    s.draw(12, 64, 384, 1.0f);
    s.draw(13, 128, 384, 1.0f);
*/

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < DOT_COUNT; ++j)
            m_dots[i][j].draw(s, frac, S_DOT);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    (void) frac;
}
