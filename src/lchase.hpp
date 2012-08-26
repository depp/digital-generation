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

    static const int NUM_MONSTER = 2,
        NUM_KEY = 2, NUM_ACTOR = NUM_MONSTER + 1;

    static const int MOVE_TICKS = 25;

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

    // LOCAL (reset when a wave is started)
    unsigned char m_tiles[WIDTH][HEIGHT];

    GameState m_state;
    int m_state_time;

    Actor m_actor[NUM_ACTOR];

    void startWave(int wave);

public:
    LChase(GameScreen &screen);
    virtual ~LChase();

    virtual void advance(unsigned time, int controls);

    virtual void draw(int frac);
};

}
#endif
