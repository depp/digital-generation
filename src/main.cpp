/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "client/ui/screen.hpp"
#include "gamescreen.hpp"
using namespace LD24;

UI::Screen *getMainScreen()
{
    return new GameScreen;
}
