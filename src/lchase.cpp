/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "lchase.hpp"
#include "client/rand.hpp"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace LD24;

static const char CHASE_LEVELS
[LChase::NUM_WAVES][LChase::HEIGHT][LChase::WIDTH+1] = {
{
    "      *        *    ",
    " ================== ",
    "    |    |  |    |  ",
    "    |    |  |    |  ",
    "    | *  |  |    |  ",
    "  =====  |  |  * |  ",
    "  |   |  |  ======  ",
    "  |   |  |     |    ",
    "  |   |  |     |    ",
    " *|   |  |  *  |    ",
    " ================== ",
}, {
    "     *              ",
    " ==========         ",
    " |        | *       ",
    " |    ========      ",
    " |    |      |      ",
    " |  * |      ====== ",
    " ======      |    | ",
    "      |    * |    | ",
    "      ========    | ",
    "         |     *  | ",
    "         ========== "
}, {
    "         *          ",
    "     ========       ",
    "  *  |      |     * ",
    "  ======  ========= ",
    "   |   |  |     |   ",
    "   |   |  |     |   ",
    " * |   |  |   * |   ",
    " =======  =======   ",
    "     |  *   |       ",
    "     ========       ",
    "                    "
}
};

LChase::LChase(GameScreen &screen)
    : Level(screen)
{
    m_tlv2 = Texture::file(LV2::TEXNAME);

    static const char FX_NAME[FX_COUNT][5] = {
        "win", "lose", "get"
    };
    for (int i = 0; i < FX_COUNT; ++i) {
        char name[16];
        std::strcpy(name, "fx/2_");
        std::strcat(name, FX_NAME[i]);
        m_fx[i] = AudioFile::file(name);
    }
    m_audio.open();

    startWave(0);
    m_beat = 0;
}

LChase::~LChase()
{ }

void LChase::startWave(int wave)
{
    int sp[6][2];

    assert(wave >= 0 && wave < NUM_WAVES);
    m_waveno = wave;

    const char (*ldata)[WIDTH+1] = CHASE_LEVELS[wave];
    int npos = 0;
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            char c = ldata[HEIGHT-y-1][x], d = ' ';
            int out = 0;

            if (y > 0) {
                d = ldata[HEIGHT-y][x];
                if (d == '=' || d == '|')
                    out |= T_PASS;
            }

            switch (c) {
            case '|':
                out |= T_PASS | T_LADDER;
                break;

            case '*':
                assert(npos < NUM_POS);
                assert(out & T_PASS);
                sp[npos][0] = x;
                sp[npos][1] = y;
                npos++;
                break;

            case '=':
                out |= T_PLATFORM;
                if (out & T_PASS)
                    out |= T_LADDER;
                break;

            case ' ':
                break;

            default:
                assert(0);
            }
            m_tiles[x][y] = out;
        }
    }
    assert(npos == NUM_POS);

    for (int i = 1; i < NUM_POS; ++i) {
        int j = Rand::girand() % (i + 1);
        if (i != j) {
            int x = sp[i][0];
            int y = sp[i][1];
            sp[i][0] = sp[j][0];
            sp[i][1] = sp[j][1];
            sp[j][0] = x;
            sp[j][1] = y;
        }
    }

    m_tiles[sp[0][0]][sp[0][1]] |= T_ITEM;
    m_tiles[sp[1][0]][sp[1][1]] |= T_ITEM;
    m_tiles[sp[2][0]][sp[2][1]] |= T_DOOR;
    for (int i = 0; i < 3; ++i) {
        m_actor[i].move = -1;
        m_actor[i].x = sp[3+i][0];
        m_actor[i].y = sp[3+i][1];
        m_actor[i].dx = 0;
        m_actor[i].dy = 0;
    }

    m_state = ST_BEGIN;
    m_state_time = 0;
}

void LChase::advance(unsigned time, int controls)
{
    m_beat = (m_beat + 1) & 0xff;
    (void) time;
    (void) controls;
}

void LChase::draw(int frac)
{
    const SpriteSheet &sp = LV2::SPRITES;

    glClearColor(0.4, 0.4, 0.4, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glScalef(4.0f, 4.0f, 4.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    m_tlv2->bind();
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBegin(GL_TRIANGLES);
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            int c = m_tiles[x][y];
            if (c & T_LADDER)
                sp.draw(LV2::LADDER, x * 16, y * 16);
            if (c & T_PLATFORM)
                sp.draw(LV2::PLAT, x * 16, y * 16);
            if (c & T_ITEM)
                sp.draw(LV2::KEY, x * 16, y * 16 - 3);
            if (c & T_DOOR)
                sp.draw(LV2::DOOR, x * 16, y * 16 - 1);
        }
    }
    for (int i = 0; i < 3; ++i) {
        int x = m_actor[i].x * 16, y = m_actor[i].y * 16;
        if (m_actor[i].move >= 0) {
            int frac2 = frac + m_actor[i].move * FRAME_TIME;
            x += (m_actor[i].dx * frac2) / (FRAME_TIME * MOVE_TICKS);
            y += (m_actor[i].dy * frac2) / (FRAME_TIME * MOVE_TICKS);
        }
        int s;
        if (i == 0)
            s = LV2::PLAYER;
        else
            s = LV2::MON1 + m_waveno * 2;
        s += ((m_beat >> 6) & 1);
        y -= 1;
        sp.draw(s, x, y);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glPopMatrix();

    (void) frac;
}
