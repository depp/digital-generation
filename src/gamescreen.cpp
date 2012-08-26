/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "defs.hpp"
#include "gamescreen.hpp"
#include "sprite.hpp"
#include "color.hpp"
#include "text.hpp"
#include "level.hpp"
#include "lball.hpp"
#include "linvade.hpp"
#include "lchase.hpp"

#include "client/ui/event.hpp"
#include "client/viewport.hpp"
#include "client/opengl.hpp"
#include "client/bitmapfont.hpp"
#include "client/keycode.hpp"

#include "base/audio_source.h"

static const unsigned LAG_THRESHOLD = 250;

using namespace LD24;

static const unsigned char KEY_MAP[] = {
    KEY_W,      CTL_UP,
    KP_8,       CTL_UP,
    KEY_Up,     CTL_UP,

    KEY_A,      CTL_LEFT,
    KP_4,       CTL_LEFT,
    KEY_Left,   CTL_LEFT,

    KEY_S,      CTL_DOWN,
    KP_5,       CTL_DOWN,
    KEY_Down,   CTL_DOWN,

    KEY_D,      CTL_RIGHT,
    KP_6,       CTL_RIGHT,
    KEY_Right,  CTL_RIGHT,

    KEY_Space,  CTL_ACTION,
    KP_0,       CTL_ACTION,

    255
};

GameScreen::GameScreen()
    : m_level(NULL),
      m_init(false),
      m_key(KEY_MAP)
{
    m_letterbox.setISize(1280, 720);
    tex_charge = Texture::file("img/charge");
}

GameScreen::~GameScreen()
{
    if (m_level)
        delete m_level;
}

unsigned GameScreen::getControls()
{
    unsigned x = 0;
    for (int i = 0; i < 5; ++i)
        if (m_key.inputState(i))
            x |= 1u << i;
    return x;
}

void GameScreen::update(unsigned ticks)
{
    if (!m_init) {
        if (!m_level) {
            // m_level = new LBall(*this);
            m_level = new LChase(*this);
            // m_level = new LInvade(*this);
        }

        m_tickref = ticks;
        m_init = true;
        m_delta = 0;
    } else {
        unsigned delta = ticks - m_tickref;
        if (delta > LAG_THRESHOLD) {
            m_tickref = ticks;
            m_delta = 0;
            m_level->advance(ticks, getControls());
        } else {
            if (delta >= (unsigned) FRAME_TIME) {
                unsigned ctl = getControls();
                unsigned frames = delta / FRAME_TIME;
                unsigned ref = m_tickref;
                for (unsigned i = 0; i < frames; ++i)
                    m_level->advance(ref + (i + 1) * FRAME_TIME, ctl);
                m_tickref = ref + FRAME_TIME * frames;
                delta -= FRAME_TIME * frames;
            }
            m_delta = delta;
        }
    }
    sg_audio_source_commit(ticks, ticks);
}

void GameScreen::drawCharge(unsigned msec, int amt)
{
    int i;
    if (amt > MAX_CHARGE)
        amt = MAX_CHARGE;
    tex_charge->bind();
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
    glColor3ub(255, 255, 255);
    SpriteSheet::CHARGE.draw(0, 80, 32, 1.0f);
    for (i = 0; i < amt; ++i) {
        float frac = i * (1.0f / (MAX_CHARGE - 1));
        Color::hsl(0.0f + 120.0f * frac, 1.0f, 0.1f + 0.5f * frac).set();
        SpriteSheet::CHARGE.draw(1 + i * 6 / 20, 80, 80 + 32 * i, 1.0f);
    }
    glColor3ub(9, 9, 9);
    for (; i < MAX_CHARGE; ++i) {
        SpriteSheet::CHARGE.draw(1 + i * 6 / 20, 80, 80 + 32 * i, 1.0f);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glColor4ub(255, 255, 255, 255);

    (void) msec;
}

void GameScreen::draw(Viewport &v, unsigned msec)
{
    (void) msec;
    (void) &v;
    m_letterbox.setOSize(v.width(), v.height());
    m_letterbox.enable();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1280, 0, 720, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (m_level)
        m_level->draw(m_delta);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    getFont().print(
        8, 8,
        "Use arrow keys or <WASD> to move. "
        "Use <space> or <enter> to perform an action.");
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    m_letterbox.disable();
}

void GameScreen::handleEvent(const UI::Event &evt)
{
    switch (evt.type) {
    case UI::KeyDown:
        /*
        switch (evt.keyEvent().key) {
        case KEY_F5:
            break;
        }
        */
    case UI::KeyUp:
        m_key.handleKeyEvent(evt.keyEvent());
        break;

    default:
        break;
    }
}
