/*                                                                    -*- c -*-
 * Copyright (C) 1999  Gus Hartmann & Peter Keller
 * Copyright (C) 2024  Ron Wills <ron@digitalcombine.ca>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "sweep.h"
#include <pwd.h>

/*************
 * set_color *
 *************/

static void set_color(game_stats_t* game, int color) {
  if (has_colors() == TRUE) {
    wcolor_set(game->Board, color, NULL);
  }
}

/***************
 * curses_init *
 ***************/

void curses_init() {
  if (isatty(STDOUT_FILENO) && isatty(STDIN_FILENO)) {
#ifdef SWEEP_MOUSE
    mmask_t Mask;
#endif
    initscr();
    if (has_colors()) {
      start_color();
      log_message("Terminal color pairs is %d", COLOR_PAIRS);
      log_message("Terminal colors available is %d", COLORS);
    }

    noecho();
    keypad(stdscr, TRUE);
    intrflush(stdscr, TRUE);
    cbreak();
    curs_set(0); // Hide the mouse

#ifdef SWEEP_MOUSE
    Mask = REPORT_MOUSE_POSITION | BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED
      | BUTTON3_CLICKED;
    Mask = mousemask(Mask, NULL);
#endif
  } else {
    fprintf(stderr, "OPPS: Freesweep cannot be played through redirected "
            "io ¯\\_(ツ)_/¯\n      Stop being so silly!\n");
    exit(EXIT_FAILURE);
  }
}

/**************
 * draw_title *
 **************/

void draw_title() {
  WINDOW* win;

  if ((win = newwin(2, INFO_W, 0, (COLS - INFO_W))) != NULL) {
    wbkgdset(win, ' ' | COLOR_PAIR(CLR_INFOBAR));
    wcolor_set(win, CLR_INFOBAR, NULL);
    wclear(win);

    mvwprintw(win, 0, 2, "-=- FreeSweep -=-");
    mvwhline(win, 1, 1, CharSet.HLine, 19);

    wnoutrefresh(win);
    delwin(win);
  } else {
    log_error("Unable to create title window");
  }
}

/***************
 * draw_splash *
 ***************/

void draw_splash() {
  unsigned int ccol = (COLS - 56) / 2;
  unsigned int cline = (LINES - 15) / 2;

  clear();

  mvprintw(cline, ccol, "   ___");
  mvprintw(cline + 1, ccol,
           " /'___\\                                      21st Century");
  mvprintw(cline + 2, ccol,
           "/\\ \\__/  _ __    __     __           Open Source Software");
  mvprintw(cline + 3, ccol,
           "\\ \\ ,__\\/\\`'__\\/'__`\\ /'__`\\");
  mvprintw(cline + 4, ccol,
           " \\ \\ \\_/\\ \\ \\//\\  __//\\  __/");
  mvprintw(cline + 5, ccol,
           "  \\ \\_\\  \\ \\_\\\\ \\____\\ \\____\\");
  mvprintw(cline + 6, ccol,
           "   \\/_/   \\/_/ \\/____/\\/____/");
  mvprintw(cline + 7, ccol,
           "                ____  __  __  __     __     __   _____");
  mvprintw(cline + 8, ccol,
           "               /',__\\/\\ \\/\\ \\/\\ \\  /'__`\\ /'__`\\/\\ '__`\\");
  mvprintw(cline + 9, ccol,
           "              /\\__, `\\ \\ \\_/ \\_/ \\/\\  __//\\  __/\\ \\ \\L\\ \\");
  mvprintw(cline + 10, ccol,
           "              \\/\\____/\\ \\___x___/'\\ \\____\\ \\____\\\\ \\ ,__/");
  mvprintw(cline + 11, ccol,
           "               \\/___/  \\/__//__/   \\/____/\\/____/ \\ \\ \\/");
  mvprintw(cline + 12, ccol,
           "                                                   \\ \\_\\");
  mvprintw(cline + 13, ccol,
           "The IT Bomb Corps                                   \\/_/");
  mvprintw(cline + 14, ccol,
           "A Questionable Career Choice");

  log_splash(50, cline + 15, (COLS - 50) / 2);
  refresh();
}

/**************
 * draw_board *
 **************/

int draw_board(game_stats_t* game) {
  int CoordX = 0, CoordY = 0, HViewable = 0, VViewable = 0;
  unsigned char CellVal;

  VViewable = ((game->LargeBoardY != 0) ? (LINES - 3) : (game->height));
  HViewable = ((game->LargeBoardX != 0) ?
               ((COLS - INFO_W - 2) / 3) : (game->width));

  for (CoordY = game->FocusY; CoordY < (game->FocusY + VViewable); CoordY++) {
    for (CoordX = game->FocusX; CoordX < (game->FocusX + HViewable);
         CoordX++) {

      wmove(game->Board,
            (CoordY - game->FocusY),
            3 * (CoordX - game->FocusX) + 1);

      CellVal = game_get_mine(game, CoordX, CoordY);
      switch (CellVal) {
      case UNKNOWN:
        set_color(game, CLR_UNKNOWN);
        xwaddch(game->Board, &CharSet, CharSet.Unknown);
        set_color(game, CLR_NORMAL);
        break;
      case EMPTY:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_SPACE, NULL);
        }
        xwaddch(game->Board, &CharSet, CharSet.Space);
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case 1:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_VAL1, NULL);
        }
        waddch(game->Board, (CellVal) + '0');
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case 2:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_VAL2, NULL);
        }
        waddch(game->Board, (CellVal) + '0');
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case 3:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_VAL3, NULL);
        }
        waddch(game->Board, (CellVal) + '0');
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case 4:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_VAL4, NULL);
        }
        waddch(game->Board, (CellVal) + '0');
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case 5:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_VAL5, NULL);
        }
        waddch(game->Board, (CellVal) + '0');
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case 6:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_VAL6, NULL);
        }
        waddch(game->Board, (CellVal) + '0');
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case 7:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_VAL7, NULL);
        }
        waddch(game->Board, (CellVal) + '0');
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case 8:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_VAL8, NULL);
        }
        waddch(game->Board, (CellVal) + '0');
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case MINE:
        if (game->Status == LOSE) {
          if (has_colors() == TRUE) {
            wcolor_set(game->Board, CLR_SHOWMINE, NULL);
          }
          xwaddch(game->Board, &CharSet, CharSet.ShowMine);
          if (has_colors() == TRUE) {
            wcolor_set(game->Board, CLR_NORMAL, NULL);
          }
        } else {
          if (has_colors() == TRUE) {
            wcolor_set(game->Board, CLR_MINE, NULL);
          }
          xwaddch(game->Board, &CharSet, CharSet.Mine);
          if (has_colors() == TRUE) {
            wcolor_set(game->Board, CLR_NORMAL, NULL);
          }
        }
        break;
      case MARKED:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_MARK, NULL);
        }
        xwaddch(game->Board, &CharSet, CharSet.Mark);
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      case BAD_MARK:
        if (game->Status == LOSE) {
          if (has_colors() == TRUE) {
            wcolor_set(game->Board, CLR_SHOWFALSEMARK, NULL);
          }
          xwaddch(game->Board,&CharSet,CharSet.ShowFalseMark);
          if (has_colors() == TRUE) {
            wcolor_set(game->Board, CLR_NORMAL, NULL);
          }
        } else {
          if (has_colors() == TRUE) {
            wcolor_set(game->Board, CLR_FALSEMARK, NULL);
          }
          xwaddch(game->Board,&CharSet,CharSet.FalseMark);
          if (has_colors() == TRUE) {
            wcolor_set(game->Board, CLR_NORMAL, NULL);
          }
        }
        break;
      case DETONATED:
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_DETONATED, NULL);
        }
        xwaddch(game->Board,&CharSet,CharSet.Bombed);
        if (has_colors() == TRUE) {
          wcolor_set(game->Board, CLR_NORMAL, NULL);
        }
        break;
      default:
        break;
      }
    }
  }
  wmove(game->Board, 0, 0);

  wnoutrefresh(game->Board);
  return 0;
}

/***************
 * cursor_draw *
 ***************/

void cursor_draw(game_stats_t* game) {
  wmove(game->Board,
        game->CursorY - game->FocusY,
        3 * (game->CursorX - game->FocusX));
  xwaddch(game->Board, &CharSet, CharSet.CursorLeft);

  wmove(game->Board,
        game->CursorY - game->FocusY,
        3 * (game->CursorX - game->FocusX) + 2);
  xwaddch(game->Board, &CharSet, CharSet.CursorRight);
}

/*****************
 * cursor_undraw *
 *****************/

void cursor_undraw(game_stats_t* game) {
  mvwaddch(game->Board,
           (game->CursorY - game->FocusY),
           (3 * (game->CursorX - game->FocusX)),
           ' ');
  mvwaddch(game->Board,
           (game->CursorY - game->FocusY),
           (3 * (game->CursorX - game->FocusX)) + 2,
           ' ');
}

/*************
 * board_pan *
 *************/

int board_pan(game_stats_t* game) {
  /* Pan() is respondsible for keeping the relevant part of the board
   * displayed.  In addition, it also draws the border, including doing any
   * "arrows" to indicate that part of the board is not presently displayed.
   * The panning itself it triggered when the cursor gets within two squares of
   * the edge of the currently displayed area.
   */
  chtype Right, Left, Top, Bottom;
  int VViewable = 0, HViewable = 0;

  Right = Left = CharSet.VLine;
  Top = Bottom = CharSet.HLine;

  /* The basic border needs to be drawn regardless. */

  /* If the board isn't larger than the screen, no need to pan. */
  if ((game->LargeBoardX + game->LargeBoardY) == 0) {
    wborder(game->Border, CharSet.VLine, CharSet.VLine,
            CharSet.HLine, CharSet.HLine, CharSet.ULCorner,
            CharSet.URCorner, CharSet.LLCorner, CharSet.LRCorner);
    wnoutrefresh(game->Border);
    return 0;
  }

  /* Now to figure out which way to go... */
  VViewable = (LINES - 3);
  HViewable = ((COLS - INFO_W - 2) / 3) + 1;

  /* See if the current cursor is even in the focus. */
  if ((game->CursorY < game->FocusY) ||
      ((game->FocusY + VViewable) < game->CursorY) ||
      ((game->CursorY - game->FocusY) < 2) ||
      (((game->FocusY + VViewable) - game->CursorY) <= 2)) {
    cursor_center_y(game);
  }

  if ((game->CursorX < game->FocusX) ||
      ((game->FocusX + HViewable) < game->CursorX) ||
      ((game->CursorX - game->FocusX) < 2) ||
      (((game->FocusX + HViewable) - game->CursorX) < 2)) {
    cursor_center_x(game);
  }

  /* Then draw the borders. */
  if (game->LargeBoardX) {
    /* Check to see if the left side needs to be an arrow boundary. */
    if (game->FocusX > 0) {
      Left = CharSet.LArrow;
    }

    /* Check to see if the right side needs to be an arrow boundary. */
    if ((game->FocusX + HViewable) < game->width) {
      Right = CharSet.RArrow;
    }
  }

  if (game->LargeBoardY) {
    /* Check to see if the top side needs to be an arrow boundary. */
    if (game->FocusY > 0) {
      Top = CharSet.UArrow;
    }

    /* Check to see if the bottom side needs to be an arrow boundary. */
    if ((game->FocusY + VViewable) < game->height) {
      Bottom = CharSet.DArrow;
    }

  }

  wborder(game->Border, Left, Right, Top, Bottom, CharSet.ULCorner,
          CharSet.URCorner, CharSet.LLCorner, CharSet.LRCorner);
  wnoutrefresh(game->Border);

  return 0;
}

/*****************
 * cursor_center *
 *****************/

int cursor_center(game_stats_t* game) {
  cursor_center_x(game);
  cursor_center_y(game);
  return 0;
}

/*******************
 * cursor_center_x *
 *******************/

int cursor_center_x(game_stats_t* game) {
  int HOffset = 0;

  if (game->LargeBoardX) {
    HOffset = (((COLS - INFO_W - 2) / 3) - 1) / 2;

    if (game->CursorX > (game->width - HOffset)) {
      game->FocusX = (game->width - (2 * HOffset) - 1);
    } else if ((game->CursorX - HOffset) >= 0) {
        game->FocusX = (game->CursorX - HOffset);
    } else {
      game->FocusX = 0;
    }
  }

  return 0;
}

/*******************
 * cursor_center_y *
 *******************/

int cursor_center_y(game_stats_t* game) {
  int VOffset = 0;

  if (game->LargeBoardY) {
    VOffset = (LINES - 3) / 2;

    if (game->CursorY > (game->height - VOffset)) {
      game->FocusY = (game->height - (LINES - 3));
    } else if ((game->CursorY - VOffset) >= 0) {
      game->FocusY = (game->CursorY - VOffset);
    } else {
      game->FocusY = 0;
    }
  }

  return 0;
}
