/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#include "zone.hpp"
#include "sprite.hpp"
#include <limits>
#include <cstdio>
using namespace LD24;

using std::vector;

Zone::Zone()
{
    m_minx = m_miny = std::numeric_limits<int>::min();
    m_maxx = m_maxy = std::numeric_limits<int>::max();
}

void Zone::reset(int ntemp, int nstatic, int nmover)
{
    m_temp.clear();
    m_static.clear();
    m_mover.clear();
    m_collision.clear();

    m_temp.resize(ntemp);
    m_static.resize(nstatic);
    m_mover.resize(nmover);
}

template<typename T>
static T *findent(vector<T> &v, int priority)
{
    typename vector<T>::iterator i = v.begin(), e = v.end();
    for (; i != e; ++i) {
        if (!i->is_active)
            return &*i;
    }
    for (; i != e; ++i) {
        if (i->priority < priority)
            return &*i;
    }
    return 0;
}

void Zone::newtemp(int sprite, int priority, int x, int y, int time)
{
    ETemp *e = findent(m_temp, priority);
    if (!e)
        return;
    e->is_active = true;
    e->sprite = sprite;
    e->priority = priority;
    e->x = x;
    e->y = y;
    e->time = time;
}

Zone::ECollide *Zone::newstatic(int sprite, int priority, int x, int y)
{
    ECollide *e = findent(m_static, priority);
    if (!e)
        return NULL;
    e->is_active = true;
    e->sprite = sprite;
    e->priority = priority;
    e->x = x;
    e->y = y;

    e->type = -1;
    e->id = -1;
    e->w = -1;
    e->h = -1;
    e->mat_mask = 0;
    return e;
}

Zone::EMover *Zone::newmover(int sprite, int priority, int x, int y)
{
    EMover *e = findent(m_mover, priority);
    if (!e)
        return NULL;
    e->is_active = true;
    e->sprite = sprite;
    e->priority = priority;
    e->x = x;
    e->y = y;
    e->vx = 0;
    e->vy = 0;
    e->px = 0;
    e->py = 0;
    e->hx = x * 256;
    e->hy = y * 256;
    return e;
}

void Zone::draw(const SpriteSheet &sp, int frac)
{
    for (vector<ECollide>::iterator
             i = m_static.begin(),
             e = m_static.end();
         i != e; ++i)
    {
        if (!i->is_active)
            continue;
        sp.draw(i->sprite, i->x, i->y);
    }

    for (vector<EMover>::iterator
             i = m_mover.begin(),
             e = m_mover.end();
         i != e; ++i)
    {
        if (!i->is_active)
            continue;
        int x = i->px + (i->x - i->px) * frac / FRAME_TIME;
        int y = i->py + (i->y - i->py) * frac / FRAME_TIME;
        // std::printf("THING %d %d\n", i->x, i->y);
        sp.draw(i->sprite, x, y);
    }

    for (vector<ETemp>::iterator
             i = m_temp.begin(),
             e = m_temp.end();
         i != e; ++i)
    {
        if (!i->is_active)
            continue;
        sp.draw(i->sprite, i->x, i->y);
    }
}

void Zone::testcol(EMover &e, ECollide &o)
{
    if ((e.col_mask & o.mat_mask) == 0)
        return;
    int rx = (e.w + o.w) / 2, ry = (e.h + o.h) / 2;
    int dx = e.x - o.x, dy = e.y - o.y;
    bool colx = dx < rx && dx > -rx;
    bool coly = dy < ry && dy > -ry;
    if (!colx || !coly)
        return;
    int mx = dx > 0 ? dx : -dx, my = dy > 0 ? dy : -dy;
    Dir dir;
    if (rx - mx > ry - my) {
        dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;
    } else {
        dir = dy > 0 ? DIR_UP : DIR_DOWN;
    }
    bool fixed = false;
    if (e.px < e.x) {
        if (e.px <= o.x - rx) {
            e.x = o.x - rx;
            e.hx = e.x * 256;
            fixed = true;
        }
    } else if (e.px > e.x) {
        if (e.px >= o.x + rx) {
            e.x = o.x + rx;
            e.hx = e.x * 256;
            fixed = true;
        }
    }
    if (!fixed) {
        if (e.py < e.y) {
            if (e.py <= o.y - ry) {
                e.y = o.y - ry;
                e.hy = e.y * 256;
                fixed = true;
            }
        } else if (e.py > e.y) {
            if (e.py >= o.y + ry) {
                e.y = o.y + ry;
                e.hy = e.y * 256;
                fixed = true;
            }
        }
    }
    // std::puts("collision");
    m_collision.push_back(Collision(&e, &o, dir));
}

void Zone::advance()
{
    m_collision.clear();

    /* Expire temporaries */
    for (vector<ETemp>::iterator
             i = m_temp.begin(),
             e = m_temp.end();
         i != e; ++i)
    {
        if (!i->is_active)
            continue;
        i->priority -= 1;
        i->time -= 1;
        if (i->time == 0)
            i->is_active = 0;
    }

    /* Move movers */
    for (vector<EMover>::iterator
             i = m_mover.begin(),
             e = m_mover.end();
         i != e; ++i)
    {
        if (!i->is_active)
            continue;
        i->hx += i->vx;
        i->hy += i->vy;
        i->px = i->x;
        i->py = i->y;
        i->x = fix2i(i->hx);
        i->y = fix2i(i->hy);
    }

    for (vector<EMover>::iterator
             xi = m_mover.begin(),
             xe = m_mover.end();
         xi != xe; ++xi)
    {
        if (!xi->is_active)
            continue;

        // Order: First walls, then statics, then movers.

        if (xi->col_mask & 1) {
            int hw = xi->w / 2, hh = xi->h / 2;
            Dir dir = DIR_LEFT;
            bool wall = false;
            if (xi->x + hw > m_maxx) {
                wall = true;
                xi->x = m_maxx - hw;
                xi->hx = xi->x * 256;
                dir = DIR_RIGHT;
            } else if (xi->x - hw < m_minx) {
                wall = true;
                xi->x = m_minx + hw;
                xi->hx = xi->x * 256;
                dir = DIR_LEFT;
            }
            if (xi->y + hh > m_maxy) {
                wall = true;
                xi->y = m_maxy - hh;
                xi->hy = xi->y * 256;
                dir = DIR_UP;
            } else if (xi->y - hh < m_miny) {
                wall = true;
                xi->y = m_miny + hh;
                xi->hy = xi->y * 256;
                dir = DIR_DOWN;
            }
            if (wall) {
                // std::printf("wall collision DIR=%d\n", (int) dir);
                m_collision.push_back(Collision(&*xi, NULL, dir));
            }
        }

        for (vector<ECollide>::iterator
                 yi = m_static.begin(),
                 ye = m_static.end();
             yi != ye; ++yi)
        {
            if (!yi->is_active)
                continue;
            testcol(*xi, *yi);
        }

        for (vector<EMover>::iterator
                 yi = m_mover.begin(),
                 ye = m_mover.end();
             yi != ye; ++yi)
        {
            if (!yi->is_active || yi == xi)
                continue;
            testcol(*xi, *yi);
        }
    }
}
