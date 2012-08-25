/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "text.hpp"
#include "client/bitmapfont.hpp"
using namespace LD24;

BitmapFont &LD24::getFont()
{
    static BitmapFont *p;
    if (!p)
        p = new BitmapFont("font/terminus-8x16");
    return *p;
}
