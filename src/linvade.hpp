/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_LINVADE_HPP
#define LD24_LINVADE_HPP
#include "defs.hpp"
#include "level.hpp"
#include "sprite.hpp"
#include "client/audio.hpp"
#include "client/texture.hpp"
namespace LD24 {

class LInvade : public Level {
private:

    Texture::Ref m_tlv3;

public:
    LInvade(GameScreen &screen);
    virtual ~LInvade();

    virtual void advance(unsigned time, int controls);

    virtual void draw(int frac);
};

}
#endif
