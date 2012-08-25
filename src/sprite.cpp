#include "sprite.hpp"
#include "client/opengl.hpp"
#include <assert.h>
using namespace LD24;

void Sprites::draw(int n, float x, float y, float scale) const
{
    assert(n >= 0 && n < count);
    const Sprite &s = sprites[n];

    float w = (float) width, h = (float) height;

    float u0 = (1.0f / w) * s.x0;
    float u1 = (1.0f / w) * s.x1;
    float v0 = (1.0f / h) * (height - s.y0);
    float v1 = (1.0f / h) * (height - s.y1);

    float x0 = x + (float) (s.x0 - (s.xc + s.x0)) * scale;
    float x1 = x + (float) (s.x1 - (s.xc + s.x0)) * scale;
    float y0 = y + (float) (s.y0 - (s.yc + s.y0)) * scale;
    float y1 = y + (float) (s.y1 - (s.yc + s.y0)) * scale;

    glTexCoord2f(u0, v0); glVertex2f(x0, y0); // 0
    glTexCoord2f(u1, v1); glVertex2f(x1, y1); // 3
    glTexCoord2f(u0, v1); glVertex2f(x0, y1); // 1

    glTexCoord2f(u0, v0); glVertex2f(x0, y0); // 0
    glTexCoord2f(u1, v0); glVertex2f(x1, y0); // 2
    glTexCoord2f(u1, v1); glVertex2f(x1, y1); // 3
}

#define ALEN(x) (sizeof(x) / sizeof(*(x)))

static const Sprites::Sprite CHARGE_SPRITES[] = {
    {   0,   0, 160,  64,  80,  32 },
    {   0,  64, 160,  96,  80,  16 },
    {   0,  96, 160, 128,  80,  16 },
    {   0, 128, 160, 160,  80,  16 },
    {   0, 160, 160, 192,  80,  16 },
    {   0, 192, 160, 224,  80,  16 },
    {   0, 224, 160, 256,  80,  16 }
};

const Sprites Sprites::CHARGE(
    256, 256, CHARGE_SPRITES, ALEN(CHARGE_SPRITES));
