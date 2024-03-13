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

static int last_key = 0;

#define CTRL(c) ((c) & 0x1f)

/*************
 * move_left *
 *************/

static void move_left(game_stats_t* game, int count) {
  if (game->CursorX >= count) {
      game->CursorX -= count;
  } else {
    log_error("Cannot move past left side of board.");
    game->CursorX = 0;
  }
}

/**************
 * move_right *
 **************/

static void move_right(game_stats_t* game, int count) {
  if ((game->CursorX + count) < game->width) {
    game->CursorX += count;
  } else {
    log_error("Cannot move past right side of board.");
    game->CursorX = (game->width - 1);
  }
}

/***********
 * move_up *
 ***********/

static void move_up(game_stats_t* game, int count) {
  if (game->CursorY >= count) {
    game->CursorY -= count;
  } else {
    log_error("Cannot move past top of board.");
    game->CursorY = 0;
  }
}

/*************
 * move_down *
 *************/

static void move_down(game_stats_t* game, int count) {
  if ((game->CursorY + count) < game->height) {
    game->CursorY += count;
  } else {
    log_error("Cannot move past bottom of board.");
    game->CursorY = game->height - 1;
  }
}

/*************
 * cell_flag *
 *************/

static void cell_flag(game_stats_t* game) {
  unsigned char RetVal;

  RetVal = game_get_mine(game, game->CursorX, game->CursorY);

  switch (RetVal) {
  case MINE:
    game_set_mine(game, game->CursorX, game->CursorY, MARKED);
    game->MarkedMines++;
    break;
  case MARKED:
    game_set_mine(game, game->CursorX, game->CursorY, MINE);
    game->MarkedMines--;
    break;
  case BAD_MARK:
    game_set_mine(game, game->CursorX, game->CursorY, UNKNOWN);
    game->BadMarkedMines--;
    break;
  case UNKNOWN:
    game_set_mine(game, game->CursorX, game->CursorY, BAD_MARK);
    game->BadMarkedMines++;
    break;
  default:
    log_error("Cannot mark as a mine.");
    break;
  }

  if (game->MarkedMines == game->mines &&
      game->BadMarkedMines == 0) {
    timer_stop();
    game->Status = WIN;
    draw_board(game);
    cursor_undraw(game);
    nodelay(game->Board, FALSE);
    wnoutrefresh(game->Board);
    refresh();
    log_message("Num %d, Marked %d, Bad %d",
                game->mines, game->MarkedMines,
                game->BadMarkedMines);
    bests_update(game);
  }
}

/***************
 * cell_expose *
 ***************/

static void cell_expose(game_stats_t* game) {
  unsigned char mine_value;

  mine_value = game_get_mine(game, game->CursorX, game->CursorY);

  switch (mine_value) {
  case BAD_MARK:
  case MARKED:
    log_error("Cannot expose a flagged mine.");
    break;
  case MINE:
    timer_stop();
    game_set_mine(game, game->CursorX, game->CursorY, DETONATED);
    game->Status = LOSE;
    break;
  case UNKNOWN:
    game_clear(game);
    break;
  case EMPTY:
    log_error("Square already exposed.");
    break;
  case 1: case 2: case 3: case 4:
  case 5: case 6: case 7: case 8:
    /* Double-click */
    game_super_clear(game);
    break;

  default:
    break;
  }
}

/***************
 * dialog_lose *
 ***************/

void dialog_lose() {
  WINDOW* win;

  if ((win = newwin(10, 29, (LINES - 10) / 2, (COLS - 29) / 2))
      == NULL) {
    log_error("Uable to create lose dialog window");
    return;
  }

  wborder(win, CharSet.VLine, CharSet.VLine, CharSet.HLine,
          CharSet.HLine, CharSet.ULCorner, CharSet.URCorner,
          CharSet.LLCorner, CharSet.LRCorner);

  mvwprintw(win, 3, 12, "Boom!");
  mvwprintw(win, 5, 6, "There goes a few");
  mvwprintw(win, 6, 11, "limbs!!");

  wrefresh(win);
  delwin(win);

  log_status("BOOM! Better luck next time.");

  flushinp();
  cbreak();
  getch();
}

/**************
 * dialog_win *
 **************/

void dialog_win() {
  WINDOW* win;

  if ((win = newwin(10, 29, (LINES - 10) / 2, (COLS - 29) / 2))
      == NULL) {
    log_error("Uable to create win dialog window");
    return;
  }

  wborder(win, CharSet.VLine, CharSet.VLine, CharSet.HLine,
          CharSet.HLine, CharSet.ULCorner, CharSet.URCorner,
          CharSet.LLCorner, CharSet.LRCorner);

  mvwprintw(win, 3, 11, "You Win!");
  mvwprintw(win, 5, 6, "Good job Sargent!");

  wrefresh(win);
  delwin(win);

  log_status("You Won! Now back to work.");

  flushinp();
  cbreak();
  getch();
}

/****************
 * dialog_abort *
 ****************/

void dialog_abort() {
  WINDOW* win;

  if ((win = newwin(10, 29, (LINES - 10) / 2, (COLS - 29) / 2))
      == NULL) {
    log_error("Uable to create abort dialog window");
    return;
  }

  wborder(win, CharSet.VLine, CharSet.VLine, CharSet.HLine,
          CharSet.HLine, CharSet.ULCorner, CharSet.URCorner,
          CharSet.LLCorner, CharSet.LRCorner);

  mvwprintw(win, 3, 6, "Mission Aborted!?");
  mvwprintw(win, 5, 5, "It's scary, I know!");

  wrefresh(win);
  delwin(win);

  log_status("Mission aborted! I know, it's a tough job!");

  flushinp();
  cbreak();
  getch();
}

/**************
 * game_input *
 **************/

int game_input(game_stats_t* game) {
  int input = 0;

#ifdef SWEEP_MOUSE
  MEVENT MouseInput;
#endif /* SWEEP_MOUSE */

  input = getch();

  if (input == '.') {
    input = last_key;
  } else {
    last_key = input;
  }

  switch (input) {
    /* Going Left? */
  case 'h':
  case KEY_LEFT:
    move_left(game, 1);
    break;

    /* Going Down? */
  case 'j':
  case KEY_DOWN:
    move_down(game, 1);
    break;

    /* Going Up? */
  case 'k':
  case KEY_UP:
    move_up(game, 1);
    break;

    /* Going Right? */
  case 'l':
  case KEY_RIGHT:
    move_right(game, 1);
    break;

    /* Diagonals. */
  case KEY_A1:
    move_up(game, 1);
    move_left(game, 1);
    break;

  case KEY_A3:
    move_up(game, 1);
    move_right(game, 1);
    break;

  case KEY_C1:
    move_down(game, 1);
    move_left(game, 1);
    break;

  case KEY_C3:
    move_down(game, 1);
    move_right(game, 1);
    break;

    /* The non-motion keys. */
    /* The accepted values for flagging a space as a mine. */
  case 'f':
  case KEY_B2:
    cell_flag(game);
    break;

    /* the accepted key to make a 'new' game */
  case 'a':
  case '\e': // Escape key
    game->Status = ABORT;
    break;

    /* The accepted keys to expose a space. */
  case ' ':
  case '\n':
  case KEY_ENTER:
  case KEY_IC:
    cell_expose(game);
    break;

    /* The accepted keys to redraw the screen. */
  case 'r':
  case CTRL('l'):
  case KEY_REFRESH:
    draw_title();
    stats_refresh();
    log_refresh();
    touchwin(game->Border);
    touchwin(game->Board);

    noutrefresh();
    break;

    /* The accepted keys to display the pause/help screen. */
  case '?':
  case KEY_HELP:
  case KEY_BREAK:
  case 'p':
    timer_stop();

    cbreak();
    pause_display();
    halfdelay(100);

    draw_title();
    log_refresh();
    stats_refresh();
    timer_start();
    break;

    /* The accepted values to center the cursor on board */
  case 'c':
    cursor_center(game);
    break;

  case 'e':
    game_find_nearest(game);
    break;

  case 'y':
    game_find_nearest_bad(game);
    break;

    /* The accepted values to suspend the game. */
  case KEY_SUSPEND:
  case 'z':
    break;

  case 'g':
    if (last_key != 'g') break;
  case KEY_HOME:
  case CTRL('a'):
  case '0':
    game->CursorX = 0;
    break;

  case KEY_END:
  case CTRL('e'):
  case '$':
    game->CursorX = game->width - 1;
    break;

  case KEY_PPAGE:
  case CTRL(KEY_HOME):
    game->CursorY = 0;
    break;

  case KEY_NPAGE:
  case CTRL(KEY_END):
  case 'G':
    game->CursorY = game->height - 1;
    break;

#ifdef SWEEP_MOUSE
  case KEY_MOUSE:
    getmouse(&MouseInput);

    if (MouseInput.bstate & BUTTON1_CLICKED) {
      int mx, my;

      mx = (MouseInput.x - 1) / 3;
      my = MouseInput.y - 1;

      if (mx < game->width && my < game->height) {
        cursor_undraw(game);
        game->CursorX = mx;
        game->CursorY = my;
        cursor_draw(game);

        cell_expose(game);
      }
    }
    if (MouseInput.bstate & BUTTON3_CLICKED) {
      int mx, my;

      mx = (MouseInput.x - 1) / 3;
      my = MouseInput.y - 1;

      if (mx < game->width && my < game->height) {
        cursor_undraw(game);
        game->CursorX = mx;
        game->CursorY = my;
        cursor_draw(game);

        cell_flag(game);
      }
    }
    break;
#endif /* SWEEP_MOUSE */

  case 'q':
    game_save(game);
    game->Status = QUIT;
    break;

  case 'i':
    game_save_image(game);
    break;

  case ERR:
    return 1;
    break;

  default:
    log_message("Got unknown keystroke: %c\n", input);
    last_key = ERR;
    return 1;
    break;
  }

  last_key = input;
  return 0;
}
