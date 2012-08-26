/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_CHASE_HPP
#define LD24_CHASE_HPP
#include "defs.hpp"
#include "level.hpp"
#include "sprite.hpp"
#include "client/audio.hpp"
#include "client/texture.hpp"
namespace LD24 {

class LChase : public Level {
public:
    static const int WIDTH = 20, HEIGHT = 11,
        NUM_WAVES = 3;

    struct Board {
        unsigned char tiles[HEIGHT][WIDTH];

        void clear();

        int get(int x, int y)
        {
            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
                return 0;
            return tiles[y][x];
        }
    };

    struct MPath {
        // List of dx/dy for a monster
        MPath()
            : delay(0), pos(0), count(0)
        { }

        void clear()
        {
            delay = 0;
            pos = 0;
            count = 0;
        }

        static const int LEN = 16;
        int delay;
        int pos, count;
        signed char moves[LEN][2];
    };

    static const int MAX_MONSTER = 4,
        MAX_ACTOR = MAX_MONSTER + 1;

    static const int MOVE_TICKS = 32;

    // Delays in each state before we can move on to the next state
    static const int TIME_WIN = SECOND * 2,
        TIME_LOSE = SECOND * 2, TIME_START = SECOND / 2;

    static const int CAPTURE_DISTANCE = 10;

    typedef enum {
        ST_BEGIN_UNPRESS,
        ST_BEGIN,
        ST_COLLECT,
        ST_LOSE,
        ST_WIN
    } GameState;

    enum {
        T_PASS = 1,
        T_LADDER = 2,
        T_PLATFORM = 4,
        T_ITEM = 8,
        T_DOOR = 16,
    };

    enum {
        FX_WIN,
        FX_LOSE,
        FX_GET
    };

    static const int FX_COUNT = FX_GET + 1;

    struct Actor {
        // -1: not moving
        // 0..MOVE_TICKS-1 -- moving
        int move;
        unsigned char x, y;
        signed char dx, dy;

        Pos curPos() const
        {
            if (move >= 0)
                return Pos(
                    16*(x-dx) + (16 * dx * move) / MOVE_TICKS,
                    16*(y-dy) + (16 * dy * move) / MOVE_TICKS);
            else
                return Pos(x * 16, y * 16);
        }
    };

private:
    // GLOBAL (not reset by a wave)
    Texture::Ref m_tlv2;
    AudioFile::Ref m_fx[FX_COUNT];
    AudioSource m_audio;
    int m_waveno;
    int m_beat;

    // LOCAL (reset when a wave is started)
    Board m_board;

    GameState m_state;
    int m_state_time;

    Actor m_actor[MAX_ACTOR];
    MPath m_path[MAX_MONSTER];
    int m_num_monster;

    Pos m_door;
    int m_num_key;

    void startWave(int wave);
    void findPath(int monster);

    void actionGet(unsigned time);
    void actionLose(unsigned time);
    void actionWin(unsigned time);
    void moveActors(unsigned time, int controls);
    void checkMonsters(unsigned time);

public:
    LChase(GameScreen &screen);
    virtual ~LChase();

    virtual void advance(unsigned time, int controls);

    virtual void draw(int frac);
};

}
#endif
