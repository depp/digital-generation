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
    static const int TEMP_LIMIT = 16,
        BARRIER_COUNT = 14,
        ASHOT_COUNT = 16,
        PSHOT_COUNT = 4,
        ALIEN_COUNT = 4,

        LEVEL_WIDTH = 1280 * 2,
        LEVEL_MINX = 0,
        LEVEL_MAXX = 1280 * 2,
        LEVEL_MINY = 32,
        LEVEL_MAXY = SCREEN_HEIGHT / 2 + 64,

        GRAVITY = 8,
        MAX_FALL = 512,
        PLAYER_MX = 256 * 1,
        PLAYER_MY = 128 * 5,
        PLAYER_ERG = PLAYER_MX / 8,
        RESPAWN_TIME = SECOND * 3,

        TANK_DX = 256 * 3/2,
        TANK_SHOTTIME = SECOND / 3,
        SHOT_SPEED = 256 * 2,
        POOF_TIME = SECOND / 4,

        NUM_WAVES = 4,
        SPAWN_TIME = SECOND * 3,
        ALIEN_MINH = 192,
        ALIEN_MAXH = SCREEN_HEIGHT / 2 - 32,
        ALIEN_SPEED = 256,
        ALIEN_SHOTTIME = SECOND * 3/2,

        CAMERA_WIDTH = SCREEN_WIDTH / 2,
        CAMERA_HEIGHT = SCREEN_HEIGHT / 2,
        CAMERA_DX = CAMERA_WIDTH / 4;

    static const int CPOOF_TIMER = SECOND / 16, CPOOF_TIME = SECOND / 3;
    static const int TPOOF_TIMER = SECOND / 8, PPOOF_TIMER = SECOND / 8;

    enum {
        MAT_SOLID = 1,
        MAT_PLAYER = 2,
        MAT_ALIEN = 4
    };

    enum {
        TYPE_BUNKER,
        TYPE_BASE,
        TYPE_ALIEN1,
        TYPE_ALIEN2,
        TYPE_ALIEN3,
        TYPE_PLAYER,
        TYPE_TANK,
        TYPE_PSHOT,
        TYPE_ASHOT
    };

    typedef enum {
        FX_ALIEN1,
        FX_ALIEN2,
        FX_ALIEN3,
        FX_BOOM1,
        FX_BOOM2,
        FX_CLICK,
        FX_DONK,
        FX_FANFARE,
        FX_LOSE,
        FX_PLINK,
        FX_SHOT,
    } Effect;

    typedef enum {
        AL_NONE,
        AL_SPAWN,
        AL_LEFT,
        AL_RIGHT,
        AL_CRASH
    } AlienState;

    typedef enum {
        ST_WALK,
        ST_WALK_AGAIN,
        ST_TANK,
        ST_DEAD,
        ST_WIN
    } LevelState;

    static const int FX_COUNT = (int) FX_SHOT + 1;

    Texture::Ref m_tlv3;
    AudioFile::Ref m_fx[FX_COUNT];
    Zone m_zone;
    LevelState m_state;
    int m_state_time;

    AudioSource m_aplayer, m_aalien, m_afx, m_aboom;

    bool m_standing;
    int m_pshot, m_pshottime, m_ashot;

    Zone::EMover *m_player, *m_tank, *m_alien[ALIEN_COUNT];
    int m_camx, m_campx;
    int m_wave, m_spawntime;
    int m_pooftime;
    AlienState m_astate[ALIEN_COUNT];
    int m_aheight[ALIEN_COUNT], m_ashottime[ALIEN_COUNT],
        m_ahealth[ALIEN_COUNT];
    int m_acount, m_ashotcount;

    void initlevel();
    void spawnPlayer(int x, int y);
    void playerCollide(unsigned time, Zone::ECollide *o, Zone::Dir dir);
    void shotCollide(unsigned time, Zone::EMover &s,
                     Zone::ECollide *o, Zone::Dir dir);
    void alienCollide(unsigned time, Zone::EMover &ent,
                      Zone::ECollide *o, Zone::Dir dir);
    void spawnAliens(unsigned time);
    void alienShoot(unsigned time, int i);
    int newPoof(Zone::ECollide *e, int ntime);

public:
    LInvade(GameScreen &screen);
    virtual ~LInvade();

    virtual void advance(unsigned time, int controls);

    virtual void draw(int frac);
};

}
#endif
