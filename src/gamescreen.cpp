#include "gamescreen.hpp"
#include "sprite.hpp"
#include "color.hpp"
#include "text.hpp"

#include "client/viewport.hpp"
#include "client/opengl.hpp"
#include "client/bitmapfont.hpp"

using namespace LD24;


GameScreen::GameScreen()
{
    m_letterbox.setISize(1280, 720);
    tex_charge = Texture::file("img/charge");
}

GameScreen::~GameScreen()
{ }

void GameScreen::update(unsigned ticks)
{
    (void) ticks;
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
    (void) &evt;
}
