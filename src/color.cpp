#include "color.hpp"
#include <cmath>
#include <cstdio>
using namespace LD24;

static int f2ub(float x)
{
    int i = (int) (255 * x);
    if (i < 0)
        i = 0;
    else if (i > 255)
        i = 255;
    return i;
}

Color Color::hsl(float h, float s, float l, float a)
{
    float h1 = std::fmod(h, 360.0f);
    if (h1 < 0)
        h1 += 360.0f;
    h1 *= 1.0f / 60.0f;
    float c = (1 - std::fabs(2 * l - 1)) * s;
    float x = c * (1 - std::fabs(std::fmod(h1, 2) - 1));
    float r, g, b;
    // std::printf("hue: %f;  c: %f;  x: %f\n", h1, c, x);
    if (h1 < 3.0f) {
        if (h1 < 1.0f) {
            r = c; g = x; b = 0;
        } else if (h1 < 2.0f) {
            r = x; g = c; b = 0;
        } else {
            r = 0; g = c; b = x;
        }
    } else {
        if (h1 < 4.0f) {
            r = 0; g = x; b = c;
        } else if (h1 < 5.0f) {
            r = x; g = 0; b = c;
        } else {
            r = c; g = 0; b = x;
        }
    }
    float m = l - 0.5f * c;
    r += m;
    g += m;
    b += m;
    // std::printf("RGB: %f %f %f\n", r, g, b);
    return Color(f2ub(r), f2ub(g), f2ub(b), f2ub(a));
}
