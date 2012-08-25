#include "client/opengl.hpp"
namespace LD24 {

struct Color {
    Color(int r, int g, int b, int a)
    {
        v[0] = r;
        v[1] = g;
        v[2] = b;
        v[3] = a;
    }

    unsigned char v[4];

    void set() const
    {
        glColor4ubv(v);
    }

    static Color hsl(float h, float s, float l, float a=1.0f);
};

}
