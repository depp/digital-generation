#include "client/ui/screen.hpp"
#include "client/ui/keymanager.hpp"
#include "client/letterbox.hpp"
#include "client/texture.hpp"

namespace LD24 {

class Level;

class GameScreen : public UI::Screen {
private:
    Letterbox m_letterbox;
    Texture::Ref tex_charge;
    Level *m_level;
    unsigned m_tickref;
    unsigned m_delta;
    bool m_init;
    UI::KeyManager m_key;

    void drawCharge(unsigned msec, int amt);
    void advance();

public:
    static const int MAX_CHARGE = 20;
    static const int FRAME_TIME = 10;

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
