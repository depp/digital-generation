/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "lchase.hpp"
#include "client/rand.hpp"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace LD24;

void LChase::Board::clear()
{
    std::memset(tiles, 0, sizeof(tiles));
}

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
    int sp[NUM_POS][2];

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
            m_board.tiles[y][x] = out;
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

    m_board.tiles[sp[0][1]][sp[0][0]] |= T_ITEM;
    m_board.tiles[sp[1][1]][sp[1][0]] |= T_ITEM;
    m_board.tiles[sp[2][1]][sp[2][0]] |= T_DOOR;
    for (int i = 0; i < 3; ++i) {
        m_actor[i].move = -1;
        m_actor[i].x = sp[3+i][0];
        m_actor[i].y = sp[3+i][1];
        m_actor[i].dx = 0;
        m_actor[i].dy = 0;
    }

    m_state = ST_BEGIN;
    m_state_time = 0;

    for (int i = 0; i < NUM_MONSTER; ++i) {
        m_path[i].clear();
        m_path[i].delay = (i + 1) * (SECOND/2);
    }
}

void LChase::findPath(int monster)
{
    /* We simply find the shortest path to the player, then chop it
       short at the first branch.  We pretend the other monster is an
       obstacle (otherwise, they might clump).  */
    static const int MAX_BRANCH = 32;

    /* Create the board, filling it with 255 for passable, 0 for
       impassable, 254 for the monster's location, and 1 for the
       player's location.  */
    Board d;
    if (0)
        std::puts("findPath");
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int c = m_board.tiles[y][x];
            d.tiles[y][x] = (c & T_PASS) ? 255 : 0;
        }
    }
    for (int i = 0; i < NUM_MONSTER; ++i) {
        if (i == monster)
            continue;
        int x = m_actor[i+1].x, y = m_actor[i+1].y;
        const MPath &path = m_path[i];
        d.tiles[y][x] = 0;
        if (1) {
            // Exclude monsters from treading each others' paths
            for (int j = path.pos; j < path.count; ++j) {
                x += path.moves[j][0];
                y += path.moves[j][0];
                d.tiles[y][x] = 0;
            }
        }
    }
    int mx = m_actor[monster+1].x, my = m_actor[monster+1].y;
    int px = m_actor[0].x, py = m_actor[0].y;
    d.tiles[my][mx] = 254;
    d.tiles[py][px] = 1;

    /* Do a simple breadth-first search for the monster from the
       player's location.  Mark each tile with the distance to the
       player, plus one.  */
    bool found = false;
    Pos front[2][MAX_BRANCH];
    int nfront = 1;
    front[0][0].x = px;
    front[0][0].y = py;
    for (int dist = 2; dist < 254 && !found; ++dist) {
        if (!nfront)
            break;
        //std::printf("nfront = %d\n", nfront);
        int n = 0;
        for (int j = 0; j < nfront; ++j) {
            Pos f = front[dist&1][j];
            for (int dir = 0; dir < 4; ++dir) {
                Pos p = Pos::fromDir(dir);
                int x = p.x + f.x, y = p.y + f.y;
                int od = d.get(x, y);
                //std::printf("od = %d\n", od);
                if (od == 254)
                    found = true;
                if (od >= 254 && n < MAX_BRANCH) {
                    Pos &dp = front[!(dist&1)][n++];
                    dp.x = x;
                    dp.y = y;
                    d.tiles[y][x] = dist;
                }
            }
        }
        nfront = n;
    }

    if (0) {
        puts("DISTANCE");
        for (int y = HEIGHT-1; y >= 0; --y) {
            for (int x = 0; x < WIDTH; ++x) {
                int c = d.tiles[y][x];
                if (!c)
                    std::fputs("   ", stdout);
                else if (c == 255)
                    std::fputs("  .", stdout);
                else
                    std::printf("%3d", c);
            }
            std::putchar('\n');
        }
    }

    /* Crawl the path forwards, generating moves.  */
    MPath &path = m_path[monster];
    if (found) {
        int delay = 0;
        int x = mx, y = my, count, dist = d.get(mx, my);
        if (0)
            std::printf("found path %d,%d to %d,%d, dist=%d\n",
                        mx, my, px, py, dist);
        for (count = 0; count < MPath::LEN; ++count) {
            int dx = 0, dy = 0, ct = 0;
            for (int dir = 0; dir < 4; ++dir) {
                Pos p = Pos::fromDir(dir);
                if (m_board.get(p.x + x, p.y + y) & T_PASS)
                    ct += 1;
                else
                    continue;
                int ndist = d.get(p.x + x, p.y + y);
                if (ndist != 0 && ndist == dist - 1) {
                    dx = p.x;
                    dy = p.y;
                }
            }
            if (!ct)
                break;
            if (ct > 2 && count > 0) {
                delay = SECOND/4;
                break;
            }
            path.moves[count][0] = dx;
            path.moves[count][1] = dy;
            dist -= 1;
            x += dx;
            y += dy;
        }
        path.pos = 0;
        path.count = count;
        path.delay = delay;
        //std::printf("found path, len=%d\n", count);
    } else {
        if (0)
            puts("Cannot find path to player, sleeping");
        path.pos = 0;
        path.count = 0;
        path.delay = SECOND/4;
    }
}


void LChase::advance(unsigned time, int controls)
{
    m_beat = (m_beat + 1) & 0xff;

    // Advance the actors
    for (int i = 0; i < NUM_ACTOR; ++i) {
        int m = m_actor[i].move;
        if (m >= 0) {
            m++;
            if (m == MOVE_TICKS) {
                // m_actor[i].x;
                // m_actor[i].y;
                m_actor[i].dx = 0;
                m_actor[i].dy = 0;
                m = -1;
            }
        }
        if (m > 0) {
            m_actor[i].move = m;
            continue;
        }
        int x = m_actor[i].x, y = m_actor[i].y;
        int dx = 0, dy = 0;
        if (i == 0) {
            int cy = 0, cx = 0;
            if (controls & FLAG_UP) cy += 1;
            if (controls & FLAG_DOWN) cy -= 1;
            if (controls & FLAG_RIGHT) cx += 1;
            if (controls & FLAG_LEFT) cx -= 1;
            if (cy && (m_board.get(x, y + cy) & T_PASS) != 0)
                dy = cy;
            else if (cx && (m_board.get(x + cx, y) & T_PASS) != 0)
                dx = cx;
        } else {
            MPath &p = m_path[i - 1];
            if (p.pos >= p.count) {
                if (!p.delay)
                    findPath(i - 1);
            }
            if (p.pos < p.count) {
                dx = p.moves[p.pos][0];
                dy = p.moves[p.pos][1];
                p.pos += 1;
            } else if (p.delay) {
                p.delay -= 1;
            }
        }
        if (dx || dy) {
            m_actor[i].move = 0;
            m_actor[i].x += dx;
            m_actor[i].y += dy;
            m_actor[i].dx = dx;
            m_actor[i].dy = dy;
        } else {
            m_actor[i].move = -1;
        }
    }

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
            int c = m_board.tiles[y][x];
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
            x -= m_actor[i].dx * 16;;
            x += (m_actor[i].dx * frac2 * 16) / (FRAME_TIME * MOVE_TICKS);
            y -= m_actor[i].dy * 16;
            y += (m_actor[i].dy * frac2 * 16) / (FRAME_TIME * MOVE_TICKS);
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
