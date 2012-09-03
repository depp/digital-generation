Digital Generation - Special Linux Version!
===========================================

This is an attempt to make a *binary* distribution of my Ludum Dare #24
game, "Digital Generation".  There are some known issues.

1. You will need to install the following libraries:

    * LibGTKGlext (Debian/Ubuntu: libgtkglext1)
    * LibASound (Debian/Ubuntu: libasound2) 
    * LibPNG (Debian/Ubuntu: libpng12-0)

2. This game has audio!  If you are experiencing problems, try turning
off Pulse Audio.  The game uses ALSA directly.  Don't get me wrong --
this is a bug in the game.

3. If the game doesn't run, you can always try compiling from source.
The main release has all of the source code necessary for compiling from
source, but it requires installing development headers, has some minor
incompatibilities with GCC 4.7, etc.  If you know that you're savvy
enough, go for it.

I am able to run this game just fine on Debian Wheezy, and I developed
it on Debian Squeeze.  This means that it will probably run on
distributions derived from Debian, such as Ubuntu and Mint.  This binary
release has also been optimized to use a minimal set of libraries.

If you want to let me know about bizarre behavior, you can get a lot of
info by running the game with the option:

    -dlog.level.root=debug

You can then copy and paste whatever information it spits out to the
terminal window.

WINE: No, the game does not work in WINE.  The images will not load.
