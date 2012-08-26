/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "linvade.hpp"
#include "client/rand.hpp"
#include <cstdio>
#include <cstring>
using namespace LD24;
using std::vector;

LInvade::LInvade(GameScreen &screen)
    : Level(screen)
{
    m_tlv3 = Texture::file(LV3::TEXNAME);

    static const char FX_NAME[FX_COUNT][8] = {
        "alien1", "alien2", "alien3", "boom1", "boom2", "click",
        "donk", "fanfare", "lose", "plink", "shot"
    };

    for (int i = 0; i < FX_COUNT; ++i) {
        char name[16];
        std::strcpy(name, "fx/3_");
        std::strcat(name, FX_NAME[i]);
        m_fx[i] = AudioFile::file(name);
    }

    m_aplayer.open();
    m_aalien.open();
    m_afx.open();

    initlevel();
}

void LInvade::spawnplayer(int x, int y)
{
    if (m_player) {
        m_player->is_active = false;
        m_player = NULL;
    }
    x = LEVEL_MAXX-256;
    Zone::EMover *e = m_zone.newmover(LV3::PLYR1, x, y);
    if (!e)
        return;
    m_player = e;
    e->type = TYPE_PLAYER;
    e->id = 0;
    e->w = 16;
    e->h = 32;
    e->mat_mask = MAT_PLAYER;
    e->col_mask = MAT_SOLID;
    m_standing = false;
}

void LInvade::initlevel()
{
    m_player = NULL;
    m_tank = NULL;
    for (int i = 0; i < ALIEN_COUNT; ++i)
        m_alien[i] = NULL;

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
            LV3::BUNK1 + (h < 128), LEVEL_MINX + x, LEVEL_MINY + y);
        if (!e)
            break;
        e->type = TYPE_BUNKER;
        e->id = i;
        e->w = 64;
        e->h = 32;
        e->mat_mask = MAT_SOLID;
    }

    {
        Zone::ECollide *e = m_zone.newstatic(
            LV3::BASE, LEVEL_MAXX - 64, LEVEL_MINY + 32);
        e->type = TYPE_BASE;
        e->id = 0;
        e->w = 64;
        e->h = 64;
        e->mat_mask = MAT_SOLID;
    }

    spawnplayer(LEVEL_MINX + 32, LEVEL_MINY + 16);

    m_campx = m_camx = LEVEL_MINX + CAMERA_WIDTH / 2;

    m_pshot = 0;
    m_pshottime = 0;
    m_ashot = 0;
}

LInvade::~LInvade()
{ }

void LInvade::playerCollide(unsigned time, Zone::ECollide *o, Zone::Dir dir)
{
    if (!m_player)
        return;
    Zone::EMover &p = *m_player;
    int fx = -1;

    switch (dir) {
    case Zone::DIR_DOWN:
        m_standing = true;
        if (p.vy < -256) {
            fx = FX_CLICK;
        }
        if (p.vy < 0)
            p.vy = 0;
        break;

    case Zone::DIR_UP:
        if (p.vy > 0)
            p.vy = 0;
        break;

    default:
        break;
    }

    if (o) {
        switch (o->type) {
        case TYPE_BASE:
            if (m_player)
                m_player->is_active = false;
            m_player = NULL;
            if (m_tank) {
                m_tank->is_active = false;
                m_tank = NULL;
            }
            {
                Zone::EMover *t = m_zone.newmover(
                    LV3::TANK, o->x - 50, o->y - 16);
                if (!t)
                    break;
                m_tank = t;
                t->type = TYPE_TANK;
                t->id = 0;
                t->w = 32;
                t->h = 32;
                t->mat_mask = MAT_PLAYER;
                t->col_mask = MAT_SOLID;
            }
            m_aplayer.stop(time);
            fx = FX_FANFARE;
            break;

        default:
            break;
        }
    }

    if (fx >= 0) {
        m_aplayer.stop(time);
        m_aplayer.play(time, *m_fx[fx], 0);
    }
}

void LInvade::pshotCollide(unsigned time, Zone::EMover &s,
                           Zone::ECollide *o, Zone::Dir dir)
{
    int fx = -1;
    s.is_active = false;
    m_pshot -= 1;
    if (!o)
        return;
    m_zone.newtemp(
        LV3::POOF2, s.x, s.y + (dir == Zone::DIR_UP ? 16 : 0),
        POOF_TIME);
    fx = FX_PLINK;

    if (fx >= 0) {
        m_afx.stop(time);
        m_afx.play(time, *m_fx[fx], 0);
    }

    (void) dir;
}

void LInvade::advance(unsigned time, int controls)
{
    int camTarget = m_camx;
    if (m_pshottime)
        m_pshottime -= 1;

    (void) time;
    (void) controls;

    int ergx = 0;
    if (controls & FLAG_LEFT)
        ergx -= 256;
    if (controls & FLAG_RIGHT)
        ergx += 256;

    if (m_player) {
        m_player->vy -= GRAVITY;
        if (m_player->vy < -MAX_FALL)
            m_player->vy = -MAX_FALL;
        if (m_standing && (controls & FLAG_UP)) {
            m_player->vy = PLAYER_MY;
        }

        int tvx = fix2i(ergx * PLAYER_MX);
        int dvx = tvx - m_player->vx;
        if (dvx < -PLAYER_ERG)
            dvx = -PLAYER_ERG;
        if (dvx > PLAYER_ERG)
            dvx = PLAYER_ERG;
        m_player->vx += dvx;
        int ptick = (time >> 7) & 1;
        if (m_standing) {
            if (m_player->vx > 0)
                m_player->sprite = LV3::PLYL1 + ptick;
            else if (m_player->vx < 0)
                m_player->sprite = LV3::PLYR1 + ptick;
        }

        camTarget = m_player->x;
    } else if (m_tank) {
        m_tank->vx = fix2i(ergx * TANK_DX);

        camTarget = m_tank->x;

        if ((controls & FLAG_ACTION) &&
            !m_pshottime &&
            m_pshot < PSHOT_COUNT)
        {
            Zone::EMover *e = m_zone.newmover(
                LV3::SHOT2, m_tank->x, m_tank->y + 34);
            if (e) {
                m_pshot++;
                m_pshottime = TANK_SHOTTIME;
                e->type = TYPE_PSHOT;
                e->id = 0;
                e->w = 16;
                e->h = 32;
                e->vy = SHOT_SPEED;
                m_aplayer.stop(time);
                m_aplayer.play(time, *m_fx[FX_SHOT], 0);
            }
        }
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
            Zone::ECollide *o = i->other;
            switch (e.type) {
            case TYPE_PLAYER:
                playerCollide(time, o, i->dir);
                break;

            case TYPE_PSHOT:
                pshotCollide(time, e, o, i->dir);
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
