===[CONTENTS]==================================================================

1 - ABOUT
2 - LICENSE/DISCLAIMER
3 - USAGE
4 - THANKS
5 - FEEDBACK
6 - DID YOU CREATE NEW LEVELS?

===[1 - ABOUT]=================================================================

apoplexy v2.0 (December 2013)
Copyright (C) 2008-2013 Norbert de Jonge <mail@norbertdejonge.nl>

A level editor for Prince of Persia 1 and 2.
The apoplexy website can be found at [ http://www.norbertdejonge.nl/apoplexy/ ].

===[2 - LICENSE/DISCLAIMER]====================================================

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see [ www.gnu.org/licenses/ ].

-----
IMPORTANT: Files in the following directories are licensed differently.
-----
pr/ -> http://sourceforge.net/projects/princed/files/PR - Graphic_Sound Editor/
ttf/ -> http://www.gnome.org/fonts/
wav/ -> http://en.wikipedia.org/wiki/Abandonware (© Jordan Mechner/Ubisoft)
png/ -> When it comes to images from the game: see above.

===[3 - USAGE]=================================================================

In order to use apoplexy, you need at least one file from the DOS version of either Prince of Persia 1 (PoP1) or Prince of Persia 2 (PoP2). Copy PoP1 (or just its "LEVELS.DAT" file) into the prince/ directory and/or copy PoP2 (or just its "PRINCE.DAT" file) into the prince2/ directory.

IMPORTANT: For PoP1, use a PRINCE.EXE that has all resources enabled in all levels. For example:

http://www.popot.org/get_the_games/software/PoP1.zip
http://www.popot.org/get_the_games/software/PoP2.zip

Optionally, you can recompile apoplexy: $ make
(You will need libsdl-image1.2-dev, libsdl-ttf2.0-dev and libsdl-gfx1.2-dev.)

Start apoplexy: $ ./apoplexy
(You will need libsdl-image1.2, libsdl-ttf2.0 and libsdl-gfx1.2.)

Instructional videos are available at:
http://www.norbertdejonge.nl/apoplexy/

===[4 - THANKS]================================================================

What: "Prince of Persia File Formats Specifications"
Thanks: Enrique Calot, Brendon James, and others

What: "Princed Resources editor"
Thanks: Enrique Calot, Santiago Zamora, and others

What: "Prince of Persia 1"
Thanks: Jordan Mechner, Avril Harrison, and others

What: "Prince of Persia 2"
Thanks: Jordan Mechner, Lance Groody, and others

What: compiling apoplexy under Windows; various suggestions
Thanks: David from the Princed Forum

===[5 - FEEDBACK]==============================================================

If apoplexy crashes, gets compilation errors or crashes while building, send an e-mail to [ mail@norbertdejonge.nl ]. Make sure to describe exactly what actions triggered the bug and the precise symptoms of the bug. Also include: 'uname -a', 'gcc -v', 'sdl-config --version', and 'apoplexy --version'.

===[6 - DID YOU CREATE NEW LEVELS?]============================================

Feel free to share your work:
http://forum.princed.org/viewforum.php?f=73
