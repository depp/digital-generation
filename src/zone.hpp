/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_ZONE_HPP
#define LD24_ZONE_HPP
#include <vector>
namespace LD24 {
class SpriteSheet;

class Zone {
public:
    typedef enum {
        DIR_LEFT,
        DIR_RIGHT,
        DIR_UP,
        DIR_DOWN
    } Dir;

    struct Entity {
        Entity()
            : is_active(false), was_active(false)
        { }

        bool is_active;
        bool was_active;
        short sprite;
        short x, y;
    };

    struct ETemp : public Entity {
        short time;
    };

    struct ECollide : public Entity {
        short type, id;
        short w, h;
        // Material mask (what is this made of)
        // Walls have mask = 1
        unsigned short mat_mask;
    };

    struct EMover : public ECollide {
        short px, py;
        short vx, vy;
        int hx, hy;
        // Collision mask (what materials does this thing collide
        // with) -- collisions happen when (col_mask & mat_mask) != 0.
        // Walls have mask = 1.
        unsigned short col_mask;
    };

    // Record of entities that collided
    struct Collision {
        Collision(EMover *e, ECollide *o, Dir d)
            : ent(e), other(o), dir(d)
        { }

        // ent is never NULL
        struct EMover *ent;
        // other may be NULL if we collide with the wall
        struct ECollide *other;
        // position of other relative to ent
        Dir dir;
    };

private:
    std::vector<ETemp> m_temp;
    std::vector<ECollide> m_static;
    std::vector<EMover> m_mover;
    std::vector<Collision> m_collision;

    int m_minx, m_maxx, m_miny, m_maxy;

    void testcol(EMover &e, ECollide &o);

public:
    Zone();

    // Resets all objects in the zone.
    // You cannot resize and keep the objects.
    void reset(int ntemp, int nstatic, int nmover);

    void newtemp(int sprite, int x, int y, int time);
    ECollide *newstatic(int sprite, int x, int y);
    EMover *newmover(int sprite, int x, int y);

    void setBounds(int x0, int x1, int y0, int y1)
    {
        m_minx = x0;
        m_maxx = x1;
        m_miny = y0;
        m_maxy = y1;
    }

    void draw(const SpriteSheet &sp, int frac);

    void advance();

    // Collisions from the most recent update
    const std::vector<Collision> &collisions() const
    {
        return m_collision;
    }
};

}
#endif
