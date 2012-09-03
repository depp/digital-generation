The Digital Generation
======================

By Dietrich Epp <depp@moria.us> for Ludum Dare #24.

http://www.ludumdare.com/compo/
http://moria.us/ludumdare/ld24/

Don't forget to vote!

http://www.ludumdare.com/compo/ludum-dare-24/?action=preview&uid=7606

Known bugs:

* OS X IMPORTANT!  The audio sounds terrible on OS X unless you set
  your sample rate to 48 kHz.  This is easy.  Open "Audio MIDI Setup"
  in the Utilities folder, select your output device, and change the
  format to 48000 Hz.  The sound still works without this, but it
  sounds really bad.

* Okay, so some of the sounds will make your ears bleed even with the
  right sample rate.  Turn the volume down before you get to level 2.

* Yes, this game can crash.  I don't know why.  It doesn't seem to
  happen often.

* The dotted lines on the edge of the screen in the first level are
  hard to see if there's lots of glare.

Linux instructions
------------------

You will need to compile this yourself.

Required packages and Debian/Ubuntu package names:

* GtkGLExt (libgtkglext1-dev)
* Pango (libpango1.0-dev)
* LibPNG (libpng12-dev)
* LibJPEG (libjpeg62-dev or libjpeg8-dev)
* ALSA (libasound-dev)

Steps:

1. Extract the source tarball.

2. Run 'configure'.

3. Run 'make'.

4. Copy the executable from "build/product/Digital_Generation" to the
   same folder that contains the "Data" directory and this README.

5. Enjoy.
