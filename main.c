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
#include <locale.h>

DrawChars CharSet;
static game_stats_t game;

/*************
 * game_exit *
 *************/

void game_exit(int result) {
  /* If somewhere in the game something fatal occurs then this is a clean
   * way to exit the program.
   */
  log_close();
  stats_close();

  clear();
  refresh();
  endwin();
  exit(result);
}

/********
 * main *
 ********/

int main(int argc, char** argv) {
  /* Set up the locale so we can support unicode characters if the terminal
   * supports it.
   */
  setlocale(LC_ALL, "");

  /* Set up the signal handlers in case of a disaster.
   */
  signal(SIGSEGV, sighandler);
  signal(SIGBUS, sighandler);
  signal(SIGILL, sighandler);
  signal(SIGWINCH, sighandler);

  /* Seed the random generator. For this game we don't really any random
   * generator more that this.
   */
  srand(time(NULL));

  game_init(&game);

  if (game_args(&game, argc, argv) > 0) {
    sleep(2);
    endwin();
    return 1;
  }

  // Start ncurses
  curses_init();
  draw_splash();

  config_load_global(&game); usleep(500000);
  config_load_user(&game);   usleep(500000);

  log_status("Starting game");
  sleep(1);

  theme_set(&game);
  if (!stats_init()) {
    game_exit(EXIT_FAILURE);
  }
  log_init(&game);
  draw_title();
  noutrefresh();
  refresh();

  while (game.Status != QUIT && game_new(&game)) {
    clear();
    log_set_alert(&game);
    cursor_center(&game);
    game.Status = INPROG;
    noutrefresh();
    doupdate();
    draw_title();

    log_message("Starting new game %dx%d with %d mines",
                game.width, game.height, game.mines);
    halfdelay(100);
    timer_start();
    /* This is the main loop.*/
    while (game.Status == INPROG) {
      stats_display(&game);
      board_pan(&game);
      cursor_draw(&game);
      draw_board(&game);
      doupdate();
      cursor_undraw(&game);
      game_input(&game);
      game.Time = g_tick;
    }
    game.Time = g_tick = 0; /* If we won, the game is already saved */
    timer_stop();
    cbreak();

    /* Update the final action of the player */
    board_pan(&game);
    cursor_draw(&game);
    draw_board(&game);
    doupdate();
    cursor_undraw(&game);

    /* see what happened */
    switch(game.Status) {
    case ABORT:
      dialog_abort();
      game_set_mines(&game);
      break;
    case WIN:
      dialog_win();
      break;
    case LOSE:
      dialog_lose();
      break;
    default:
      break;
    }

    flushinp();
  }

  log_message("Shutting down the game");
  log_close();
  stats_close();
  endwin();

  return 0;
}
