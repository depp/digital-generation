#include "gamescreen.hpp"
#include "sprite.hpp"

#include "client/viewport.hpp"
#include "client/opengl.hpp"

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

    tex_charge->bind();
    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
    Sprites::CHARGE.draw(0, 80, 32, 1.0f);
    for (int i = 0; i < (msec % 1000) / 50; ++i) {
        Sprites::CHARGE.draw(1 + i * 6 / 20, 80, 80 + 32 * i, 1.0f);
    }
    Sprites::CHARGE.draw(0, 80, 32, 1.0f);
    glEnd();

    m_letterbox.disable();
}

void GameScreen::handleEvent(const UI::Event &evt)
{
    (void) &evt;
}
