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
        NUM_WAVES = 3, NUM_POS = 6;

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

    static const int NUM_MONSTER = 2,
        NUM_KEY = 2, NUM_ACTOR = NUM_MONSTER + 1;

    static const int MOVE_TICKS = 32;

    typedef enum {
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
        T_PLAYER = 32,
        T_MONSTER = 64
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

    Actor m_actor[NUM_ACTOR];
    MPath m_path[NUM_MONSTER];

    void startWave(int wave);
    void findPath(int monster);

    void actionGet(unsigned time);
    void actionLose(unsigned time);
    void actionWin(unsigned time);

public:
    LChase(GameScreen &screen);
    virtual ~LChase();

    virtual void advance(unsigned time, int controls);

    virtual void draw(int frac);
};

}
#endif
