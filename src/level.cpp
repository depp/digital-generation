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

void Level::setTipFlags(unsigned flags)
{
    if (flags == m_tipflag)
        return;
    levelTips.clear();
    for (int i = 0; i < 32; ++i) {
        if (flags & (1u << i))
            levelTips.push_back(m_allTips[i]);
    }
}

void Level::addTip(int index)
{
    setTipFlags(m_tipflag | (1u << index));
}

void Level::removeTip(int index)
{
    setTipFlags(m_tipflag & ~(1u << index));
}
