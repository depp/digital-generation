namespace LD24 {
class GameScreen;

class Level {
public:
    /* Controls */
    enum {
        CTL_LEFT,
        CTL_RIGHT,
        CTL_UP,
        CTL_DOWN,
        CTL_ACTION
    };

    enum {
        FLAG_LEFT = 1 << CTL_LEFT,
        FLAG_RIGHT = 1 << CTL_RIGHT,
        FLAG_UP = 1 << CTL_UP,
        FLAG_DOWN = 1 << CTL_DOWN,
        FLAG_ACTION = 1 << CTL_ACTION
    };

    GameScreen &screen;

    Level(GameScreen &s)
        : screen(s)
    { }

    virtual ~Level();

    /* Advance one frame, 10 milliseconds.  The controls are the FLAG
       constant above, combined to show which buttons the user has
       down.  */
    virtual void advance(int controls) = 0;

    /* Draw the frame (frac is milliseconds since 'advance') */
    virtual void draw(unsigned frac) = 0;
};

}
