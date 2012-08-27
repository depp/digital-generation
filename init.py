#!/usr/bin/env python
import os
import sys
root = sys.path[0]
sys.path.insert(1, os.path.join(root, 'sglib', 'scripts'))
from gen.project import *
p = Project(root)

p.info.set(
    PKG_NAME        = 'Digital Generation',
    PKG_IDENT       = 'us.moria.diggen',
    PKG_EMAIL       = 'depp@zdome.net',
    PKG_URL         = 'http://moria.us/ludumdare/ld24/',
    PKG_COPYRIGHT   = u'Copyright \xa9 2012 Dietrich Epp',
    DEFAULT_CVARS   = [
        ('log.level.root', 'debug'),
        ('log.winconsole', 'yes'),
        ('path.data', Path('data')),
    ],
)

p.add_module(Executable(
    'DIGGEN', 'Digital Generation executable',
    reqs = 'SGLIB SGLIBXX',
    EXE_NAME = 'Digital Generation',
    EXE_MACICON = 'Icon',
    EXE_APPLE_CATEGORY = 'public.app-category.arcade-games',
))

p.add_sourcelist('Game', os.path.join('src', 'srclist.txt'), 'DIGGEN')
p.add_sourcelist_str('Resources', 'resources',
"""\
Icon.icns MACOSX
""", 'DIGGEN')


p.run()
