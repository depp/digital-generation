/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "gamescreen.hpp"
#include "sgpp/ui/screen.hpp"
using namespace LD24;

UI::Screen *getMainScreen()
{
    return new GameScreen;
}
