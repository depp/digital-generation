/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "linvade.hpp"
#include "client/rand.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace LD24;
using std::vector;

#define CHEAT 0

static const char *const LINVADE_TIPS[] = {
    "Use the arrow keys or <WASD> to move, "
    "press <SPACE> to fire tank cannon.",

    "The base on the right edge has tanks you can use.",

    "Goal: defeat all aliens."
};

LInvade::LInvade(GameScreen &screen)
    : Level(screen, LINVADE_TIPS)
{
    m_tlv3 = Texture::file(LV3::TEXNAME);
    m_moon = Texture::file("img/moon");

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
    m_aboom.open();

    initlevel();

    addTip(0);
    addTip(1);
    addTip(2);
}

void LInvade::spawnPlayer(int x, int y)
{
    if (m_player) {
        m_player->is_active = false;
        m_player = NULL;
    }
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
    m_state = ST_WALK;
    m_state_time = 0;
    m_player = NULL;
    m_tank = NULL;
    for (int i = 0; i < ALIEN_COUNT; ++i)
        m_alien[i] = NULL;
    m_acount = 0;
    m_ashotcount = 0;

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
        y = y*3/16;
        x = (x-128)*3/16;
        if (i == 0 || i == BARRIER_COUNT - 1) {
            x = 0;
            y = 0;
        }
        x += barrier_step / 2 + barrier_step * i;
        // std::printf("y = %d\n", y);
        y += 64;
        
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

    if (CHEAT)
        spawnPlayer(LEVEL_MAXX - 128, LEVEL_MINY + 16);
    else
        spawnPlayer(LEVEL_MINX + 32, LEVEL_MINY + 16);

    m_campx = m_camx = LEVEL_MINX + CAMERA_WIDTH / 2;

    m_pshot = 0;
    m_pshottime = 0;
    m_ashot = 0;
    m_wave = (CHEAT) ? 3 : 0;

    m_spawntime = SPAWN_TIME;
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
            if (m_state == ST_WALK || m_state == ST_WALK_AGAIN) {
                if (m_tank) {
                    m_tank->is_active = false;
                    m_tank = NULL;
                }
                Zone::EMover *t = m_zone.newmover(
                    LV3::TANK, o->x - 50, o->y - 16);
                if (!t)
                    break;
                m_tank = t;
                if (m_player)
                    m_player->is_active = false;
                m_player = NULL;
                t->type = TYPE_TANK;
                t->id = 0;
                t->w = 32;
                t->h = 32;
                t->mat_mask = MAT_PLAYER;
                t->col_mask = MAT_SOLID;
                m_aplayer.stop(time);
                fx = FX_FANFARE;
                m_state = ST_TANK;
            }
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

void LInvade::shotCollide(unsigned time, Zone::EMover &s,
                          Zone::ECollide *o, Zone::Dir dir)
{
    int fx = -1;
    bool player = s.type == TYPE_PSHOT;
    s.is_active = false;
    if (player)
        m_pshot -= 1;
    else
        m_ashotcount -= 1;
    if (!o)
        return;

    switch (o->type) {
    case TYPE_BUNKER:
        if (player)
            fx = FX_PLINK;
        break;

    case TYPE_ALIEN1:
    case TYPE_ALIEN2:
    case TYPE_ALIEN3:
        if (player) {
            int a = o->id;
            fx = FX_DONK;
            if (m_ahealth[a] > 0 && m_alien[a]) {
                m_ahealth[a]--;
                if (!m_ahealth[a]) {
                    fx = o->type == TYPE_ALIEN3 ? FX_BOOM2 : FX_BOOM1;
                    m_astate[a] = AL_CRASH;
                    m_ashottime[a] = 1;
                    m_alien[a]->vx =
                        fix2i(2 * ALIEN_SPEED * (rand8() -128));
                    m_alien[a]->vy = -ALIEN_SPEED;
                }
            }
        }
        break;

    case TYPE_PLAYER:
        if (!player && m_player && m_state != ST_DEAD) {
            m_state = ST_DEAD;
            m_aplayer.stop(time);
            m_aplayer.play(time, *m_fx[FX_LOSE], 0);
            m_state_time = RESPAWN_TIME;
            if (m_tank) {
                m_tank->is_active = false;
                m_tank = NULL;
            }
            m_pooftime = 1;
        }
        break;

    case TYPE_TANK:
        if (!player && !m_player && m_tank && m_state == ST_TANK) {
            fx = FX_BOOM1;
            m_state = ST_WALK_AGAIN;
            m_pooftime = 1;
            m_tank->vx = 0;
            int x = m_tank->x;
            if (x > (LEVEL_MINX + LEVEL_MAXX) / 2)
                x -= 32;
            else
                x += 32;
            spawnPlayer(x, 16);
        }
        break;

    default:
        break;
    }

    int y = s.y;
    int ns;
    if (player) {
        if (y < o->y - o->h/2)
            y = o->y - o->h/2;
        ns = LV3::POOF2;
    } else {
        if (y > o->y + o->h/2)
            y = o->y + o->h/2;
        ns = LV3::POOF1;
    }
    m_zone.newtemp(ns, s.x, y, POOF_TIME);

    if (fx >= 0) {
        AudioSource &s =
            (fx == FX_BOOM1 || fx == FX_BOOM2) ? m_aboom : m_afx;
        s.stop(time);
        s.play(time, *m_fx[fx], 0);
    }

    (void) dir;
}

void LInvade::alienCollide(unsigned time, Zone::EMover &ent,
                           Zone::ECollide *o, Zone::Dir dir)
{
    int a = ent.id;
    if (!m_alien[a])
        return;
    switch (m_astate[a]) {
    case AL_CRASH:
        m_alien[a]->is_active = false;
        m_alien[a] = NULL;
        m_acount -= 1;
        if (!m_acount) {
            m_spawntime = SECOND / 2;
        }
        break;

    default:
        break;
    }

    (void) time;
    (void) o;
    (void) dir;
}

void LInvade::spawnAliens(unsigned time)
{
    int wave = m_wave + 1;
    if (wave > NUM_WAVES) {
        m_state = ST_WIN;
        return;
    }
    m_wave = wave;
    bool spawned = false;
    int t1 = 0, t2 = 0;
    switch (wave) {
    case 1: t1 = 0; t2 = 0; break;
    case 2: t1 = 0; t2 = 1; break;
    case 3: case 4: t1 = 1; t2 = 1; break;
    }
    for (int i = 0; i < ALIEN_COUNT; ++i) {
        if (m_alien[i])
            continue;
        int t = (i & 1) ? t2 : t1;
        if (wave == 4 && i == 0)
            t = 2;
        int y = LEVEL_MAXY - 32;
        int spread = (LEVEL_MAXX - LEVEL_MINX) / ALIEN_COUNT;
        int x = spread / 2 + i * spread;
        Zone::EMover *e = m_zone.newmover(LV3::SHIP1 + t, x, y);
        if (!e)
            break;
        spawned = true;
        m_alien[i] = e;
        e->type = TYPE_ALIEN1 + t;
        e->id = i;
        e->w = 32;
        e->h = 16;
        if (t == 2) {
            e->w = 64;
            e->h = 32;
        }
        e->vy = -256;
        e->mat_mask = MAT_ALIEN;
        e->col_mask = MAT_SOLID;
        m_astate[i] = AL_SPAWN;
        m_aheight[i] = ALIEN_MINH +
            (int) (Rand::gfrand() * (ALIEN_MAXH - ALIEN_MINH));
        m_ashottime[i] = fix2i(ALIEN_SHOTTIME * (64 + rand8()/2));
        m_acount += 1;
        m_ahealth[i] = 1 << t;
    }
    if (spawned) {
        m_aalien.stop(time);
        m_aalien.play(time, *m_fx[FX_ALIEN1], 0);
    }
}

void LInvade::alienShoot(unsigned time, int a) {
    m_ashottime[a] = fix2i(ALIEN_SHOTTIME * (128 + rand8()));
    int c = 1;
    bool didshoot = false;
    if (!m_alien[a])
        return;
    switch (m_alien[a]->type) {
    case TYPE_ALIEN1: c = 1; break;
    case TYPE_ALIEN2: c = 3; break;
    case TYPE_ALIEN3: c = 4; break;
    }
    for (int i = 0; i < c; ++i) {
        if (m_ashotcount >= ASHOT_COUNT)
            break;
        int x = m_alien[a]->x - (c - 1) * 8 + i * 16;
        int s;
        if (i * 2 > c - 1)
            s = LV3::SHOT1R;
        else if (i * 2 < c - 1)
            s = LV3::SHOT1L;
        else
            s = LV3::SHOT1C;
        Zone::EMover *e = m_zone.newmover(s, x, m_alien[a]->y - 16);
        if (!e)
            break;
        didshoot = true;
        m_ashotcount += 1;
        e->type = TYPE_ASHOT;
        e->id = 0;
        e->w = 16;
        e->h = 16;
        e->vy = -SHOT_SPEED;
        e->vx = SHOT_SPEED * (2*i - c + 1) / 2;
        e->mat_mask = 0;
        e->col_mask = MAT_SOLID | MAT_PLAYER;
    }
    if (didshoot) {
        m_aalien.stop(time);
        m_aalien.play(time, *m_fx[FX_SHOT], 0);
    }
}

int LInvade::newPoof(Zone::ECollide *e, int ntime)
{
    if (!e)
        return 999;
    unsigned r = Rand::girand();
    int rx = (int) ((r >> 4) & 255) - 128;
    int ry = (int) ((r >> 12) & 255) - 128;
    m_zone.newtemp(
        LV3::POOF1,
        e->x + fix2i(e->w * rx),
        e->y + fix2i(e->h * ry),
        CPOOF_TIME);
    return fix2i(((int) ((r >> 20) & 255) + 128) * ntime);
}

void LInvade::advance(unsigned time, int controls)
{
    // std::printf("player = %p\n", (void *) m_player);
    if (m_state == ST_DEAD) {
        controls = 0;
        if (m_state_time && !--m_state_time) {
            initlevel();
        } else {
            if (--m_pooftime < 0)
                m_pooftime = newPoof(m_player, PPOOF_TIMER);
        }
    } else if (m_state == ST_WIN) {
        if (--m_spawntime < 0)
            nextLevel();
    } else {
        if (m_spawntime && !--m_spawntime)
            spawnAliens(time);
    }

    int camTarget = m_camx;
    if (m_pshottime)
        m_pshottime -= 1;

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

        if (--m_pooftime < 0 && m_tank) {
            m_pooftime = newPoof(m_tank, TPOOF_TIMER);
        }
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
                e->col_mask = MAT_ALIEN | MAT_SOLID;
                e->mat_mask = 0;
                m_aplayer.stop(time);
                m_aplayer.play(time, *m_fx[FX_SHOT], 0);
            }
        }
    }

    {
        m_campx = m_camx;
        bool movecamera = false;
        if (0) {
            int delta = camTarget - m_camx;
            if (0)
                std::printf("camx: %d; target: %d; delta %d\n",
                            m_camx, camTarget, delta);
            if (delta > CAMERA_DX) {
                camTarget -= CAMERA_DX;
                movecamera = true;
            } else if (delta < -CAMERA_DX) {
                camTarget += CAMERA_DX;
                movecamera = true;
            }
        } else {
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

    for (int i = 0; i < ALIEN_COUNT; ++i) {
        if (!m_alien[i])
            continue;

        switch (m_astate[i]) {
        case AL_NONE:
            break;

        case AL_SPAWN:
            if (m_alien[i]->y <= m_aheight[i]) {
                m_alien[i]->vy = 0;
                goto alien_chdir;
            }
            break;

        case AL_LEFT:
            if (m_alien[i]->x < m_camx - CAMERA_WIDTH / 2 + ALIEN_MARGIN)
                goto alien_chdir;
            goto alien_shoot;

        case AL_RIGHT:
            if (m_alien[i]->x > m_camx + CAMERA_WIDTH / 2 - ALIEN_MARGIN)
                goto alien_chdir;
            goto alien_shoot;

        alien_chdir:
            {
                int r = (Rand::girand() >> 8) & 255;
                int speed = fix2i(ALIEN_SPEED * (r + 128));
                if (std::abs(m_alien[i]->x - m_camx) > CAMERA_WIDTH * 3 / 4)
                    speed *= 2;
                if (m_alien[i]->x < m_camx) {
                    m_alien[i]->vx = speed;
                    m_astate[i] = AL_RIGHT;
                } else {
                    m_alien[i]->vx = -speed;
                    m_astate[i] = AL_LEFT;
                }
            }
            break;

        alien_shoot:
            if (--m_ashottime[i] <= 0)
                alienShoot(time, i);
            break;

        case AL_CRASH:
            if (--m_ashottime[i] <= 0)
                m_ashottime[i] = newPoof(m_alien[i], CPOOF_TIMER);
            break;
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
            Zone::EMover &ent = *i->ent;
            Zone::ECollide *o = i->other;
            switch (ent.type) {
            case TYPE_PLAYER:
                playerCollide(time, o, i->dir);
                break;

            case TYPE_PSHOT:
            case TYPE_ASHOT:
                shotCollide(time, ent, o, i->dir);
                break;

            case TYPE_ALIEN1:
            case TYPE_ALIEN2:
            case TYPE_ALIEN3:
                alienCollide(time, ent, o, i->dir);
                break;
            }
        }
    }
}

void LInvade::draw(int frac)
{
    const SpriteSheet &sp = LV3::SPRITES;
    int camx = m_campx + ((m_camx - m_campx) * frac) / FRAME_TIME;

    glClearColor(0.02, 0.03, 0.09, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glScalef(2.0f, 2.0f, 2.0f);
    glTranslatef((float) (CAMERA_WIDTH/2 - camx), 0, 0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    {
        int moonw = 1024*2, moonh = 128*2;
        int mmin = moonw / 2, mmax = LEVEL_WIDTH - moonw / 2;
        int cmin = CAMERA_WIDTH, cmax = LEVEL_WIDTH - CAMERA_WIDTH;
        int x = mmin + ((mmax - mmin) * (camx - cmin)) / (cmax - cmin);

        float x0 = (float) (x - moonw / 2);
        float x1 = (float) (x + moonw / 2);
        float y0 = (float) LEVEL_MINY;
        float y1 = (float) (LEVEL_MINY + moonh);

        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
        m_moon->bind();
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x0, y0);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x1, y0);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x0, y1);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x1, y1);
        glEnd();
    }

    m_tlv3->bind();
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
