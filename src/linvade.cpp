/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "linvade.hpp"
using namespace LD24;

LInvade::LInvade(GameScreen &screen)
    : Level(screen)
{
    m_tlv3 = Texture::file(LV3::TEXNAME);
}

LInvade::~LInvade()
{ }

void LInvade::advance(unsigned time, int controls)
{
    (void) time;
    (void) controls;
}

void LInvade::draw(int frac)
{
    const SpriteSheet sp = LV3::SPRITES;

    glClearColor(0.3, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    m_tlv3->bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < LV3::COUNT; ++i) {
        int x = (i & 7) + 1;
        int y = (i >> 3) + 1;
        sp.draw(i, x * 128 + 48, y * 128 + 48);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
     glDisable(GL_BLEND);

    (void) frac;
}
