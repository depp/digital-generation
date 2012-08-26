/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "sprite.hpp"
#include "client/opengl.hpp"
#include <assert.h>
using namespace LD24;

void SpriteSheet::draw(int n, float x, float y, float sx, float sy) const
{
    assert(n >= 0 && n < count);
    const Sprite &s = sprites[n];

    float w = (float) width, h = (float) height;

    float u0 = (1.0f / w) * s.x0;
    float u1 = (1.0f / w) * s.x1;
    float v0 = (1.0f / h) * (height - s.y0);
    float v1 = (1.0f / h) * (height - s.y1);



    float x0, x1, y0, y1;
    if (s.x0 <= s.x1) {
        x0 = (float) (s.x0 - (s.xc + s.x0)) * sx;
        x1 = (float) (s.x1 - (s.xc + s.x0)) * sx;
    } else {
        x0 = (float) (s.x1 - (s.xc + s.x1)) * sx;
        x1 = (float) (s.x0 - (s.xc + s.x1)) * sx;
    }
    x0 += x;
    x1 += x;

    if (s.y0 <= s.y1) {
        y0 = (float) (s.y0 - (s.yc + s.y0)) * sy;
        y1 = (float) (s.y1 - (s.yc + s.y0)) * sy;
    } else {
        y0 = (float) (s.y1 - (s.yc + s.y1)) * sy;
        y1 = (float) (s.y0 - (s.yc + s.y1)) * sy;
    }
    y0 += y;
    y1 += y;

    glTexCoord2f(u0, v0); glVertex2f(x0, y0); // 0
    glTexCoord2f(u1, v1); glVertex2f(x1, y1); // 3
    glTexCoord2f(u0, v1); glVertex2f(x0, y1); // 1

    glTexCoord2f(u0, v0); glVertex2f(x0, y0); // 0
    glTexCoord2f(u1, v0); glVertex2f(x1, y0); // 2
    glTexCoord2f(u1, v1); glVertex2f(x1, y1); // 3
}

#define ALEN(x) (sizeof(x) / sizeof(*(x)))

static const SpriteSheet::Sprite CHARGE_SPRITES[] = {
    {   0,   0, 160,  64,  80,  32 },
    {   0,  64, 160,  96,  80,  16 },
    {   0,  96, 160, 128,  80,  16 },
    {   0, 128, 160, 160,  80,  16 },
    {   0, 160, 160, 192,  80,  16 },
    {   0, 192, 160, 224,  80,  16 },
    {   0, 224, 160, 256,  80,  16 }
};

const SpriteSheet SpriteSheet::CHARGE(
    256, 256, CHARGE_SPRITES, ALEN(CHARGE_SPRITES));

static const SpriteSheet::Sprite LV1_SPRITES[] = {
{   4, 180,  44, 252,  20,   4 },
{  44, 180,  84, 252,  20,   4 },
{  84, 180, 124, 252,  20,   4 },
{ 124, 180, 164, 252,  20,   4 },
{ 164, 180, 204, 252,  20,   4 },
{   4, 100,  44, 172,  20,   4 },
{  44, 100,  84, 172,  20,   4 },
{  84, 100, 124, 172,  20,   4 },
{ 124, 100, 164, 172,  20,   4 },
{ 164, 100, 204, 172,  20,   4 },
{ 228, 148, 252, 252,  12,  52 },
{ 228, 116, 252, 140,  12,  12 },
{   4,  20,  44,  92,   4,   4 },
{  52,  20, 172,  92,   4,   4 },
{ 228,  68, 252, 108,  12,  12 }
};

const SpriteSheet SpriteSheet::LV1(
    256, 256, LV1_SPRITES, ALEN(LV1_SPRITES));

static const SpriteSheet::Sprite LV2_SPRITES[LV2::COUNT] = {
    {  0, 16, 16, 32, 0, 0 },
    { 16, 16, 32, 32, 0, 0 },
    { 32, 16, 48, 32, 0, 0 },
    { 48, 16, 64, 32, 0, 0 },
    {  0,  0, 16, 16, 0, 0 },
    { 16,  0,  0, 16, 0, 0 },
    { 16,  0, 32, 16, 0, 0 },
    { 32,  0, 16, 16, 0, 0 },
    { 32,  0, 48, 16, 0, 0 },
    { 48,  0, 32, 16, 0, 0 },
    { 48,  0, 64, 16, 0, 0 },
    { 64,  0, 48, 16, 0, 0 }
};

const char LV2::TEXNAME[] = "img/lv2";

const SpriteSheet LV2::SPRITES(
    64, 32, LV2_SPRITES, ALEN(LV2_SPRITES));

static const SpriteSheet::Sprite LV3_SPRITES[LV3::COUNT] = {
{   0,   0,  64,  64,  32,  32 }, // base
{   0,  64,  64,  96,  32,  16 }, // bunk1
{   0,  96,  64, 128,  32,  16 }, // bunk2
{  64,   0,  80,  32,   8,  16 }, // plyr1
{  80,   0,  96,  32,   8,  16 }, // plyr2
{  80,   0,  64,  32,   8,  16 }, // plyl1
{  96,   0,  80,  32,   8,  16 }, // plyl2
{  96,   0, 128,  32,  16,  16 }, // tank
{  96,  48,  80,  64,   8,   8 }, // shot1l
{  64,  48,  80,  64,   8,   8 }, // shot1c
{  80,  48,  96,  64,   8,   8 }, // shot1r
{  80,  32,  96,  48,   8,   8 }, // poof1
{  96,  32, 112,  64,   8,  16 }, // shot2
{ 112,  32, 128,  64,   8,  16 }, // poof2
{  64,  64,  96,  96,  16,  16 }, // ship1
{  96,  64, 128,  96,  16,  16 }, // ship2
{  64,  96, 128, 128,  32,  16 }, // ship3
{  64,  32,  80,  48,   8,   8 }, // kitr
{  80,  32,  64,  48,   8,   8 }  // kitl
};

const char LV3::TEXNAME[] = "img/lv3";

const SpriteSheet LV3::SPRITES(
    128, 128, LV3_SPRITES, ALEN(LV3_SPRITES));
