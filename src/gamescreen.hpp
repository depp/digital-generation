/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_GAMESCREEN_HPP
#define LD24_GAMESCREEN_HPP
#include "sgpp/ui/screen.hpp"
#include "sgpp/ui/keymanager.hpp"
#include "sgpp/letterbox.hpp"
#include "sgpp/texture.hpp"

namespace LD24 {

class Level;

class GameScreen : public UI::Screen {
private:
    Letterbox m_letterbox;
    Level *m_level;
    unsigned m_tickref;
    unsigned m_delta;
    bool m_init;
    UI::KeyManager m_key;
    int m_lvchange;
    int m_lvno;

    int m_tip;
    unsigned m_tiptick;

    void advance(unsigned ticks, int controls);
    unsigned getControls();

public:

    GameScreen();
    virtual ~GameScreen();

    virtual void update(unsigned ticks);
    virtual void draw(Viewport &v, unsigned ticks);
    virtual void handleEvent(const UI::Event &evt);

    /* Convert to local coordinates */
    void convert(int &x, int &y)
    {
        m_letterbox.convert(x, y);
    }

    void nextLevel()
    {
        m_lvchange = m_lvno + 1;
    }
};

} // namespace LD24
#endif
