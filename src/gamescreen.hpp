#include "client/ui/screen.hpp"
#include "client/letterbox.hpp"
#include "client/texture.hpp"

namespace LD24 {

class GameScreen : public UI::Screen {
private:
    Letterbox m_letterbox;
    Texture::Ref tex_charge;

    void drawCharge(unsigned msec, int amt);

public:
    static const int MAX_CHARGE = 20;

    GameScreen();
    virtual ~GameScreen();

    virtual void update(unsigned ticks);
    virtual void draw(Viewport &v, unsigned ticks);
    virtual void handleEvent(const UI::Event &evt);

    /* Convert to local coordinates */
    void convert(int &x, int &y)
    {
        m_letterbox.convert(x, y);
    }
};

} // namespace LD24
