#include "level.hpp"
namespace LD24 {

class LBall {
public:
    LBall();
    virtual ~LBall();

    virtual void advance(int controls);

    virtual void draw(unsigned frac);
}

}
