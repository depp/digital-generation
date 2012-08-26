/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_LINVADE_HPP
#define LD24_LINVADE_HPP
#include "defs.hpp"
#include "level.hpp"
#include "sprite.hpp"
#include "zone.hpp"
#include "client/audio.hpp"
#include "client/texture.hpp"
namespace LD24 {

class LInvade : public Level {
private:
    static const int TEMP_LIMIT = 8,
        BARRIER_COUNT = 12,
        ASHOT_COUNT = 16,
        PSHOT_COUNT = 4,
        ALIEN_COUNT = 4,

        LEVEL_WIDTH = 1280 * 2,
        LEVEL_MINX = 0,
        LEVEL_MAXX = 1280 * 2,
        LEVEL_MINY = 32,
        LEVEL_MAXY = 720,

        GRAVITY = 64,
        MAX_FALL = 512,
        PLAYER_MX = 256,
        PLAYER_MY = 256;

    enum {
        TYPE_BUNKER,
        TYPE_BASE,
        TYPE_ALIEN1,
        TYPE_ALIEN2,
        TYPE_ALIEN3,
        TYPE_PLAYER,
        TYPE_TANK,
        TYPE_SHOT
    };

    Texture::Ref m_tlv3;
    Zone m_zone;

    Zone::EMover *m_eplayer, *m_etank, *m_ealien[ALIEN_COUNT];

    void initlevel();

public:
    LInvade(GameScreen &screen);
    virtual ~LInvade();

    virtual void advance(unsigned time, int controls);

    virtual void draw(int frac);
};

}
#endif