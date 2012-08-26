/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "ltitle.hpp"
#include "text.hpp"
#include "client/rand.hpp"
#include "client/opengl.hpp"
#include "client/bitmapfont.hpp"
#include <cstring>
using namespace LD24;

LTitle::LTitle(GameScreen &screen,
               const char *title, bool is_end)
    : Level(screen), m_title(title), m_is_end(is_end),
      m_time(INTER_TIME)
{
    int i, l = 0, maxl = 0;
    for (i = 0; title[i]; ++i) {
        if (title[i] == '\n') {
            if (l > maxl)
                maxl = l;
            l = 0;
        } else {
            l++;
        }
    }
    if (l > maxl)
        maxl = l;
    m_width = maxl;
}

LTitle::~LTitle()
{ }

void LTitle::advance(unsigned time, int controls)
{
    if (m_is_end)
        return;
    if (m_time && --m_time)
        return;
    if (controls & FLAG_ACTION)
        nextLevel();
    (void) time;
}

void LTitle::draw(int frac)
{
    (void) frac;

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
    glScalef(4.0f, 4.0f, 4.0f);
    getFont().print(-m_width * 4, -8, m_title);
    glPopMatrix();
}
