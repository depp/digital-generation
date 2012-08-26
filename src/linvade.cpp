/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "linvade.hpp"
#include "client/rand.hpp"
#include <cstdio>
using namespace LD24;

void LInvade::initlevel()
{
    m_zone.reset(
        TEMP_LIMIT,
        BARRIER_COUNT + 1,
        PSHOT_COUNT + ASHOT_COUNT + ALIEN_COUNT + 2);
    m_zone.setBounds(
        LEVEL_MINX, LEVEL_MAXX, LEVEL_MINY, LEVEL_MAXY);

    for (int i = 0; i < BARRIER_COUNT; ++i) {
        unsigned r = Rand::girand();
        int h = ((r >> 4) & 127) + 64;
        int x = (r >> 12) & 255;
        int y = (r >> 20) & 255;
        x = (x - 128) / 4 + 128 + 256 * i;
        // std::printf("y = %d\n", y);
        y = y/4 + 64;
        
        Zone::ECollide *e = m_zone.newstatic(
            LV3::BUNK1 + (h < 128), 0, LEVEL_MINX + x, LEVEL_MINY + y);
        if (!e)
            break;
        e->type = TYPE_BUNKER;
        e->id = i;
        e->w = 64;
        e->h = 32;
        e->mat_mask = 1;
    }

    {
        Zone::EMover *e;
        m_eplayer = e = m_zone.newmover(
            LV3::PLYR1, 999, LEVEL_MINX + 32, LEVEL_MINY + 48);
        if (!e)
            return;
        e->type = TYPE_PLAYER;
        e->id = 0;
        e->w = 16;
        e->h = 32;
        e->mat_mask = 1;
        e->col_mask = 1;
    }

    m_etank = NULL;
}

LInvade::LInvade(GameScreen &screen)
    : Level(screen)
{
    m_tlv3 = Texture::file(LV3::TEXNAME);

    m_eplayer = NULL;
    m_etank = NULL;
    for (int i = 0; i < ALIEN_COUNT; ++i)
        m_ealien[i] = NULL;

    initlevel();
}

LInvade::~LInvade()
{ }

void LInvade::advance(unsigned time, int controls)
{
    (void) time;
    (void) controls;

    if (m_eplayer) {
        m_eplayer->vy -= GRAVITY;
        if (m_eplayer->vy < -MAX_FALL)
            m_eplayer->vy = -MAX_FALL;
    }

    m_zone.advance();
}

void LInvade::draw(int frac)
{
    const SpriteSheet sp = LV3::SPRITES;

    glClearColor(0.02, 0.03, 0.09, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    m_tlv3->bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLES);
    /*
    for (int i = 0; i < LV3::COUNT; ++i) {
        int x = (i & 7) + 1;
        int y = (i >> 3) + 1;
        sp.draw(i, x * 128 + 48, y * 128 + 48);
    }
    */
    m_zone.draw(sp, frac);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    (void) frac;
}
