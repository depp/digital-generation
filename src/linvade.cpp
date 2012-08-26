/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "linvade.hpp"
#include "client/rand.hpp"
#include <cstdio>
using namespace LD24;
using std::vector;

LInvade::LInvade(GameScreen &screen)
    : Level(screen)
{
    m_tlv3 = Texture::file(LV3::TEXNAME);

    initlevel();
}

void LInvade::spawnplayer(int x, int y)
{
    if (m_eplayer) {
        m_eplayer->is_active = false;
        m_eplayer = NULL;
    }
    Zone::EMover *e = m_zone.newmover(LV3::PLYR1, 999, x, y);
    if (!e)
        return;
    m_eplayer = e;
    e->type = TYPE_PLAYER;
    e->id = 0;
    e->w = 16;
    e->h = 32;
    e->mat_mask = 1;
    e->col_mask = 1;
    m_standing = false;
}

void LInvade::initlevel()
{
    m_eplayer = NULL;
    m_etank = NULL;
    for (int i = 0; i < ALIEN_COUNT; ++i)
        m_ealien[i] = NULL;

    m_zone.reset(
        TEMP_LIMIT,
        BARRIER_COUNT + 1,
        PSHOT_COUNT + ASHOT_COUNT + ALIEN_COUNT + 2);
    m_zone.setBounds(
        LEVEL_MINX, LEVEL_MAXX, LEVEL_MINY, LEVEL_MAXY);

    int barrier_step = (LEVEL_MAXX - LEVEL_MINX) / (BARRIER_COUNT + 2);
    for (int i = 0; i < BARRIER_COUNT; ++i) {
        unsigned r = Rand::girand();
        int h = ((r >> 4) & 127) + 64;
        int x = (r >> 12) & 255;
        int y = (r >> 20) & 255;
        if (i == 0 || i == BARRIER_COUNT - 1) {
            x = 0;
            y = 0;
        }
        x = (x - 128) / 4 + barrier_step / 2 + barrier_step * i;
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

    spawnplayer(LEVEL_MINX + 32, LEVEL_MINY + 16);

    m_campx = m_camx = LEVEL_MINX + CAMERA_WIDTH / 2;
}

LInvade::~LInvade()
{ }

void LInvade::advance(unsigned time, int controls)
{
    int camTarget = m_camx;

    (void) time;
    (void) controls;

    if (m_eplayer) {
        m_eplayer->vy -= GRAVITY;
        if (m_eplayer->vy < -MAX_FALL)
            m_eplayer->vy = -MAX_FALL;
        if (m_standing && (controls & FLAG_UP)) {
            m_eplayer->vy = PLAYER_MY;
        }

        int ergx = 0;
        if (controls & FLAG_LEFT)
            ergx -= 256;
        else if (controls & FLAG_RIGHT)
            ergx += 256;
        int tvx = fix2i(ergx * PLAYER_MX);
        int dvx = tvx - m_eplayer->vx;
        if (dvx < -PLAYER_ERG)
            dvx = -PLAYER_ERG;
        if (dvx > PLAYER_ERG)
            dvx = PLAYER_ERG;
        m_eplayer->vx += dvx;
        int ptick = (time >> 7) & 1;
        if (m_standing) {
            if (m_eplayer->vx > 0)
                m_eplayer->sprite = LV3::PLYL1 + ptick;
            else if (m_eplayer->vx < 0)
                m_eplayer->sprite = LV3::PLYR1 + ptick;
        }

        camTarget = m_eplayer->x;
    }

    {
        m_campx = m_camx;
        int delta = camTarget - m_camx;
        if (0)
            std::printf("camx: %d; target: %d; delta %d\n",
                        m_camx, camTarget, delta);
        bool movecamera = false;
        if (delta > CAMERA_DX) {
            camTarget -= CAMERA_DX;
            movecamera = true;
        } else if (delta < -CAMERA_DX) {
            camTarget += CAMERA_DX;
            movecamera = true;
        }
        if (movecamera) {
            if (camTarget < LEVEL_MINX + CAMERA_WIDTH / 2)
                camTarget = LEVEL_MINX + CAMERA_WIDTH / 2;
            else if (camTarget > LEVEL_MAXX - CAMERA_WIDTH / 2)
                camTarget = LEVEL_MAXX - CAMERA_WIDTH / 2;
            m_camx = camTarget;
        }
    }

    m_standing = false;
    m_zone.advance();

    {
        const vector<Zone::Collision> &cols = m_zone.collisions();
        for (vector<Zone::Collision>::const_iterator
                 i = cols.begin(), e = cols.end();
             i != e; ++i)
        {
            Zone::EMover &e = *i->ent;
            // Zone::ECollide *o = i->other;
            switch (e.type) {
            case TYPE_PLAYER:
                switch (i->dir) {
                case Zone::DIR_DOWN:
                    m_standing = true;
                    if (e.vy < 0)
                        e.vy = 0;
                    // PLAY SOUND
                    break;

                case Zone::DIR_UP:
                    if (e.vy > 0)
                        e.vy = 0;
                    break;

                default:
                    break;
                }
                break;
            }
        }
    }
}

void LInvade::draw(int frac)
{
    const SpriteSheet sp = LV3::SPRITES;
    int camx = m_campx + ((m_camx - m_campx) * frac) / FRAME_TIME;

    glClearColor(0.02, 0.03, 0.09, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glScalef(2.0f, 2.0f, 2.0f);
    glTranslatef((float) (CAMERA_WIDTH/2 - camx), 0, 0);
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
    glPopMatrix();

    (void) frac;
}
