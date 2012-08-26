/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "level.hpp"
#include "gamescreen.hpp"
using namespace LD24;

Level::~Level()
{ }

void Level::nextLevel()
{
    screen.nextLevel();
}
