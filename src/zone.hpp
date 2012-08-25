/* Copyright 2012 Dietrich Epp <depp@zdome.net> */
#ifndef LD24_ZONE_HPP
#define LD24_ZONE_HPP
namespace LD24 {

class Zone {
public:
    enum {
        FLAG_NOHIT = 1
    };

    struct Entity {
        short type, num;
        short w, h;
        short x, y, px, py;
        short sprite;
        unsigned short flags;
        short vx, vy;
        int hrx, hry;
    };

};

}
#endif
