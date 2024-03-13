/*                                                                    -*- c -*-
 * Copyright (C) 1999  Gus Hartmann & Peter Keller
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

extern game_stats_t game;
volatile unsigned int g_tick = 0;

/**************
 * sighandler *
 **************/

void sighandler(int signo) {
  switch(signo) {
  case SIGALRM:
    g_tick++;
    signal(SIGALRM, sighandler);
    alarm(1);
    return;
    break;

  case SIGSEGV:
    clear();
    refresh();
    endwin();
    printf("\n\t***Segmentation Fault detected. Cleaning up....\n\n");
    exit(EXIT_FAILURE);
    break;

  case SIGBUS:
    clear();
    refresh();
    endwin();
    printf("\n\t***Bus Error detected. Cleaning up....\n\n");
    exit(EXIT_FAILURE);
    break;

  case SIGILL:
    clear();
    refresh();
    endwin();
    printf("\n\t***Illegal Instruction detected. Cleaning up....\n\n");
    exit(EXIT_FAILURE);
    break;

  case SIGWINCH:
    // XXX This is still very broken :(
    endwin(); // Recreate stdscr
    clear();
    noutrefresh();
    refresh();

    draw_title();
    stats_resize();
    game_resize(&game);

    noutrefresh();
    doupdate();
    break;
  default:
    break;
  }

  signal(SIGALRM, sighandler);
}
