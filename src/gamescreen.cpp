/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "defs.hpp"
#include "gamescreen.hpp"
#include "sprite.hpp"
#include "text.hpp"
#include "level.hpp"
#include "lball.hpp"
#include "linvade.hpp"
#include "lchase.hpp"
#include "ltitle.hpp"

#include "client/ui/event.hpp"
#include "client/viewport.hpp"
#include "client/opengl.hpp"
#include "client/bitmapfont.hpp"
#include "client/keycode.hpp"

#include "base/audio_source.h"

static const char *const TMSG[] = {
    "Level 1\n"
    "\n"
    "I grew up in a four-bit\n"
    "microcontroller on the\n"
    "wrong side of town...\n",

    "Level 2\n"
    "\n"
    "Sometimes you can change\n"
    "with the times...",

    "Level 3\n"
    "\n"
    "I could hardly keep up\n"
    "with these new things!",

    "But,\n"
    "I managed in the end.\n"
    "\n"
    "      For Ludum Dare #24\n"
    "         by Dietrich Epp"
};

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
      m_key(KEY_MAP),
      m_lvchange(-1),
      m_lvno(0)
{
    m_letterbox.setISize(1280, 720);
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

void GameScreen::advance(unsigned ticks, int controls)
{
    if (m_lvchange > 7)
        m_lvchange = -1;
    if (m_lvchange >= 0) {
        if (m_level) {
            delete m_level;
            m_level = NULL;
        }
        switch (m_lvchange) {
        case 1: m_level = new LTitle(*this, TMSG[0], false); break;
        case 2: m_level = new LBall(*this); break;
        case 3: m_level = new LTitle(*this, TMSG[1], false); break;
        case 4: m_level = new LChase(*this); break;
        case 5: m_level = new LTitle(*this, TMSG[2], false); break;
        case 6: m_level = new LInvade(*this); break;
        case 7: m_level = new LTitle(*this, TMSG[3], true); break;
        default: assert(0);
        }
        m_lvno = m_lvchange;
        m_lvchange = -1;
    }
    assert(m_level != NULL);
    m_level->advance(ticks, controls);
}

void GameScreen::update(unsigned ticks)
{
    if (!m_init) {
        m_lvchange = m_lvno + 1;
        m_tickref = ticks;
        m_init = true;
        m_delta = 0;
    } else {
        unsigned delta = ticks - m_tickref;
        if (delta > LAG_THRESHOLD) {
            m_tickref = ticks;
            m_delta = 0;
            advance(ticks, getControls());
        } else {
            if (delta >= (unsigned) FRAME_TIME) {
                unsigned ctl = getControls();
                unsigned frames = delta / FRAME_TIME;
                unsigned ref = m_tickref;
                for (unsigned i = 0; i < frames; ++i)
                    advance(ref + (i + 1) * FRAME_TIME, ctl);
                m_tickref = ref + FRAME_TIME * frames;
                delta -= FRAME_TIME * frames;
            }
            m_delta = delta;
        }
    }
    sg_audio_source_commit(ticks, ticks);
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
        switch (evt.keyEvent().key) {
        case KEY_F5:
            nextLevel();
            break;
        }
    case UI::KeyUp:
        m_key.handleKeyEvent(evt.keyEvent());
        break;

    default:
        break;
    }
}
