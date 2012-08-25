#include "gamescreen.hpp"
#include "sprite.hpp"
#include "color.hpp"
#include "text.hpp"
#include "level.hpp"
#include "lball.hpp"

#include "client/ui/event.hpp"
#include "client/viewport.hpp"
#include "client/opengl.hpp"
#include "client/bitmapfont.hpp"
#include "client/keycode.hpp"

static const unsigned LAG_THRESHOLD = 250;

using namespace LD24;

static const unsigned char KEY_MAP[] = {
    KEY_W,      Level::CTL_UP,
    KP_8,       Level::CTL_UP,
    KEY_Up,     Level::CTL_UP,

    KEY_A,      Level::CTL_LEFT,
    KP_4,       Level::CTL_LEFT,
    KEY_Left,   Level::CTL_LEFT,

    KEY_S,      Level::CTL_DOWN,
    KP_5,       Level::CTL_DOWN,
    KEY_Down,   Level::CTL_DOWN,

    KEY_D,      Level::CTL_RIGHT,
    KP_6,       Level::CTL_RIGHT,
    KEY_Right,  Level::CTL_RIGHT,

    KEY_Space,  Level::CTL_ACTION,
    KP_0,       Level::CTL_ACTION,

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

void GameScreen::advance()
{
    
}

void GameScreen::update(unsigned ticks)
{
    if (!m_init) {
        m_tickref = ticks;
        m_init = true;
        m_delta = 0;

        if (!m_level) {
            m_level = 
        }
    } else {
        unsigned delta = ticks - m_tickref;
        if (delta > LAG_THRESHOLD) {
            m_tickref = ticks;
            m_delta = 0;
        } else {
            if (delta >= (unsigned) FRAME_TIME) {
                unsigned frames = delta / FRAME_TIME;
                m_tickref += frames * FRAME_TIME;
                for (unsigned i = 0; i < frames; ++i)
                    advance();
                delta -= FRAME_TIME * frames;
            }
            m_delta = delta;
        }
    }

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
    Sprites::CHARGE.draw(0, 80, 32, 1.0f);
    for (i = 0; i < amt; ++i) {
        float frac = i * (1.0f / (MAX_CHARGE - 1));
        Color::hsl(0.0f + 120.0f * frac, 1.0f, 0.1f + 0.5f * frac).set();
        Sprites::CHARGE.draw(1 + i * 6 / 20, 80, 80 + 32 * i, 1.0f);
    }
    glColor3ub(9, 9, 9);
    for (; i < MAX_CHARGE; ++i) {
        Sprites::CHARGE.draw(1 + i * 6 / 20, 80, 80 + 32 * i, 1.0f);
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

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    getFont().print(200, 10, "Hello, world");

    drawCharge(msec, (msec % 1000) / 48);

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
