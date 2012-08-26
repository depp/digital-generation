/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "lchase.hpp"
#include "client/rand.hpp"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace LD24;

#define CHEAT 1

void LChase::Board::clear()
{
    std::memset(tiles, 0, sizeof(tiles));
}

static const char CHASE_LEVELS
[LChase::NUM_WAVES][LChase::HEIGHT][LChase::WIDTH+1] = {
{
    "   *  EM            ",
    "   ========         ",
    "    |   *|          ",
    "    |   =========   ",
    "    |*   |   * |    ",
    "======   |   =======",
    "  |      |    |     ",
    "  |    P |    |     ",
    "  ==============    ",
    "                    ",
    "                    "
}, {
    "  E  M              ",
    " ==========         ",
    " |     |  | *       ",
    " |    ========      ",
    " |* M |      |  *   ",
    "=======      =======",
    "      |      |    | ",
    "      | *    |    | ",
    "      ========    | ",
    "         |  |  P  | ",
    "         ========== "
}, {
    "      M E  M        ",
    "     ========       ",
    "     |*     | *     ",
    "   =====  =======   ",
    " * |   |* |     |   ",
    "====   ====     ====",
    "   |*  |  |   * |   ",
    "   =====  =======   ",
    "     |  P   |       ",
    "     ========       ",
    "                    "
}, {
    "  *   M    *   M    ",
    "====================",
    "    |     |      |  ",
    "    |     |      |  ",
    "    | *  ====    |  ",
    "  =====  |  |  * |  ",
    "  |   |  |  ======  ",
    "  |   |  |     |    ",
    "  |   ====     |    ",
    " P|    |    *  |  E ",
    "====================",
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
    assert(wave >= 0 && wave < NUM_WAVES);
    m_waveno = wave;

    const char (*ldata)[WIDTH+1] = CHASE_LEVELS[wave];
    int nkey = 0, nmonster = 0;
    bool has_door = false, has_player = false;
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

            case 'E':
                assert(!has_door);
                has_door = true;
                m_door = Pos(x, y);
                break;

            case 'M':
                assert(nmonster < MAX_MONSTER);
                m_actor[nmonster+1].pos = Pos(x, y);
                nmonster++;
                break;

            case 'P':
                assert(!has_player);
                has_player = true;
                m_actor[0].pos = Pos(x, y);
                break;

            case '*':
                out |= T_ITEM;
                nkey += 1;
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
            m_board[Pos(x, y)] = out;
        }
    }
    assert(has_door && has_player && nkey > 0 && nmonster > 0);
    m_num_key = nkey;
    m_num_monster = nmonster;

    for (int i = 0; i < nmonster + 1; ++i) {
        m_actor[i].move = -1;
        m_actor[i].delta = Pos(0, 0);
    }

    m_state = ST_BEGIN_UNPRESS;
    m_state_time = TIME_START;

    for (int i = 0; i < nmonster; ++i) {
        m_path[i].clear();
        m_path[i].delay = 0;//(i + 1) * (SECOND/2);
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
            Pos p(x, y);
            int c = m_board[p];
            d[p] = (c & T_PASS) ? 255 : 0;
        }
    }
    for (int i = 0; i < m_num_monster; ++i) {
        if (i == monster)
            continue;
        Pos p = m_actor[i+1].pos;
        const MPath &path = m_path[i];
        d[p] = 0;
        if (1) {
            // Exclude monsters from treading each others' paths
            for (int j = path.pos; j < path.count; ++j) {
                p = clipPos(p + path.moves[j]);
                d[p] = 0;
            }
        }
    }
    Pos mpos = m_actor[monster+1].pos, ppos = m_actor[0].pos;
    d[mpos] = 254;
    d[ppos] = 1;

    /* Do a simple breadth-first search for the monster from the
       player's location.  Mark each tile with the distance to the
       player, plus one.  */
    bool found = false;
    Pos front[2][MAX_BRANCH];
    int nfront = 1;
    front[0][0] = ppos;
    for (int dist = 2; dist < 254 && !found; ++dist) {
        if (!nfront)
            break;
        //std::printf("nfront = %d\n", nfront);
        int n = 0;
        for (int j = 0; j < nfront; ++j) {
            Pos f = front[dist&1][j];
            for (int dir = 0; dir < 4; ++dir) {
                Pos p = movePos(f, dir);
                int od = d.get(p);
                //std::printf("od = %d\n", od);
                if (od == 254)
                    found = true;
                if (od >= 254 && n < MAX_BRANCH) {
                    front[!(dist&1)][n++] = p;
                    d[p] = dist;
                }
            }
        }
        nfront = n;
    }

    if (0) {
        puts("DISTANCE");
        for (int y = HEIGHT-1; y >= 0; --y) {
            for (int x = 0; x < WIDTH; ++x) {
                int c = d[Pos(x, y)];
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
        int count, dist = d[mpos];
        if (0)
            std::printf("found path %d,%d to %d,%d, dist=%d\n",
                        mpos.x, mpos.y, ppos.x, ppos.y, dist);
        for (count = 0; count < MPath::LEN; ++count) {
            Pos delta(0, 0);
            int ct = 0;
            for (int dir = 0; dir < 4; ++dir) {
                Pos dd = Pos::fromDir(dir);
                Pos p = clipPos(mpos + dd);
                if (m_board.get(p) & T_PASS)
                    ct += 1;
                else
                    continue;
                int ndist = d.get(p);
                if (ndist != 0 && ndist == dist - 1)
                    delta = dd;
            }
            if (!ct)
                break;
            if (ct > 2 && count > 0) {
                delay = SECOND/4;
                break;
            }
            path.moves[count] = delta;
            dist -= 1;
            mpos += delta;
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
void LChase::actionGet(unsigned time)
{
    if (m_state != ST_COLLECT)
        return;
    m_audio.stop(time);
    m_audio.play(time, *m_fx[FX_GET], 0);
    m_num_key -= 1;
    if (!m_num_key) {
        m_board[m_door] |= T_DOOR;
    }
}

void LChase::actionLose(unsigned time)
{
    if (m_state != ST_COLLECT)
        return;
    m_audio.stop(time);
    m_audio.play(time, *m_fx[FX_LOSE], 0);
    m_state = ST_LOSE;
    m_state_time = TIME_LOSE;
}

void LChase::actionWin(unsigned time)
{
    if (m_state != ST_COLLECT)
        return;
    m_audio.stop(time);
    m_audio.play(time, *m_fx[FX_WIN], 0);
    m_state = ST_WIN;
    m_state_time = TIME_WIN;
}

void LChase::advance(unsigned time, int controls)
{
    m_beat = (m_beat + 1) & 0xff;

    if (CHEAT && !m_beat && (controls & FLAG_ACTION))
        startWave(m_waveno + 1);

    switch (m_state) {
    case ST_BEGIN_UNPRESS:
    case ST_BEGIN:
        if (!controls)
            m_state = ST_BEGIN;
        if (m_state_time) {
            m_state_time--;
        } else if (m_state == ST_BEGIN) {
            int f = FLAG_LEFT | FLAG_RIGHT | FLAG_UP | FLAG_DOWN;
            if (controls & f) {
                m_state = ST_COLLECT;
                break;
            }
        }
        break;

    case ST_COLLECT:
        break;

    case ST_LOSE:
        if (!m_state_time--)
            startWave(m_waveno);
        break;

    case ST_WIN:
        if (!m_state_time--)
            startWave(m_waveno + 1);
        break;

    default:
        assert(0);
    }

    if (m_state == ST_COLLECT) {
        moveActors(time, controls);
        checkMonsters(time);
    }
}

void LChase::moveActors(unsigned time, int controls)
{
    for (int i = 0; i < m_num_monster + 1; ++i) {
        int move = m_actor[i].move;
        Pos pos = m_actor[i].pos;
        if (move >= 0) {
            move++;
            if (move == MOVE_TICKS) {
                m_actor[i].delta = Pos(0, 0);
                move = -1;
            } else if (move == (MOVE_TICKS+1)/2) {
                if (i == 0) {
                    int c = m_board.get(pos);
                    if (c & T_ITEM) {
                        m_board[pos] = c & ~T_ITEM;
                        actionGet(time);
                    } else if (c & T_DOOR) {
                        actionWin(time);
                    }
                }
            }
        }
        if (move > 0) {
            m_actor[i].move = move;
            continue;
        }
        Pos delta(0, 0);
        if (i == 0) {
            Pos ctl(0, 0);
            if (controls & FLAG_UP)    ctl.y += 1;
            if (controls & FLAG_DOWN)  ctl.y -= 1;
            if (controls & FLAG_RIGHT) ctl.x += 1;
            if (controls & FLAG_LEFT)  ctl.x -= 1;
            if (ctl.y && (m_board.get(pos.x, pos.y + ctl.y) & T_PASS))
                delta.y = ctl.y;
            else if (ctl.x && (m_board.get(pos.x + ctl.x, pos.y) & T_PASS))
                delta.x = ctl.x;
        } else {
            MPath &p = m_path[i - 1];
            if (p.pos >= p.count) {
                if (!p.delay)
                    findPath(i - 1);
            }
            if (p.pos < p.count) {
                delta = p.moves[p.pos];
                p.pos += 1;
            } else if (p.delay) {
                p.delay -= 1;
            }
        }
        if (delta) {
            m_actor[i].move = 0;
            m_actor[i].pos = clipPos(pos + delta);
            m_actor[i].delta = delta;
        } else {
            m_actor[i].move = -1;
        }
    }
}

void LChase::checkMonsters(unsigned time)
{
    Pos ppos = m_actor[0].curPos();
    for (int i = 0; i < m_num_monster; ++i) {
        Pos mpos = m_actor[i+1].curPos();
        int dx = std::abs(ppos.x - mpos.x);
        int dy = std::abs(ppos.y - mpos.y);
        int dist = dx > dy ? dx : dy;
        if (dist <= CAPTURE_DISTANCE) {
            if (0) {
                std::printf(
                    "dist %d; mon %d,%d; ply %d,%d; #%d\n",
                    dist, mpos.x, mpos.y, ppos.x, ppos.y, i);
                for (int j = 0; j < m_num_monster + 1; ++j) {
                    Actor &a = m_actor[j];
                    std::printf("pos %d,%d; delta %d,%d; move %d\n",
                                a.pos.x, a.pos.y,
                                a.delta.x, a.delta.y, a.move);
                }
            }
            actionLose(time);
            return;
        }
    }
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
            int c = m_board[Pos(x, y)];
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

    int beat = ((m_beat >> 6) & 1);
    bool visible = true;
    int frac2 = (m_state == ST_COLLECT) ? frac : 0;
    if (m_state == ST_BEGIN || m_state == ST_BEGIN_UNPRESS)
        visible = beat == 1;
    if (visible) {
        for (int i = 0; i < m_num_monster + 1; ++i) {
            Pos p = m_actor[i].curPos(frac2);
            int s;
            if (i == 0)
                s = LV2::PLAYER;
            else
                s = LV2::MON1 + (m_waveno % 3) * 2;
            s += beat;
            p.y -= 1;
            sp.draw(s, p.x, p.y);
            if (p.x < 0)
                sp.draw(s, p.x + WIDTH * 16, p.y);
            else if (p.x >= WIDTH * 16)
                sp.draw(s, p.x - WIDTH * 16, p.y);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glPopMatrix();
}
