# freesweep

Freesweep is a console minesweeper-style game written in C for Unix-like
systems. The ncurses library is preferred, but standard System V curses will
work almost perfectly. Features include boards up to 1024x1024, saving and
loading of boards, shared and individual "best times" files, and color. 

Freesweep is maintained by Gus Hartmann <gus@gus-the-cat.org>, Peter Keller
<psilord@cs.wisc.edu>, and Randy Westlund <rwestlun@gmail.com>.

## License

Freesweep is licensed under the GNU GPL, version 2 or above. See the file
COPYING, run freesweep with the -g flag, or hit 'g' while playing the game to
see it. The code is copyright Gus Hartmann and Peter Keller.

## Notes

- Bugs should be filed at https://github.com/rwestlund/freesweep/issues

- Color works, but right now the values are not customizable. Sorry.

- If no file locking mechanism is found, group-writable best times will be
  disabled.

- While the game is playable on any terminal supported by curses/ncurses, the
  help and high scores windows are designed to use a screen 80 columns wide.

Special thanks to Pete Keller for lots of help.
Thanks to the FSF for the license and certain chunks of code.
Thanks to the Undergraduate Project Lab at the University of Wisconsin
