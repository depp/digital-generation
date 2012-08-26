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

private:
    // GLOBAL (not reset by a wave)
    Texture::Ref m_tlv2;
    AudioFile::Ref m_fx[FX_COUNT];
    AudioSource m_audio;
    int m_waveno;

    // LOCAL (reset when a wave is started)
    unsigned char m_tiles[WIDTH][HEIGHT];
    int m_starpos[NUM_POS][2];

    GameState m_state;
    int m_state_time;

    void startWave(int wave);

public:
    LChase(GameScreen &screen);
    virtual ~LChase();

    virtual void advance(unsigned time, int controls);

    virtual void draw(int frac);
};

}
#endif
