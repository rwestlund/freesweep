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

static WINDOW* stats_win;

/***************
 * stats_clear *
 ***************/

static void stats_clear() {
  if (stats_win) {
    werase(stats_win);
  }
}

/**************
 * stats_init *
 **************/

int stats_init() {
  if ((stats_win = newwin(LINES - 3, INFO_W, 2, (COLS - INFO_W))) == NULL) {
    log_error("Failed to create stats window");
    return 0;
  }

  wbkgdset(stats_win, ' ' | COLOR_PAIR(CLR_STATBAR));
  wclear(stats_win);
  wrefresh(stats_win);
  return 1;
}

/****************
 * stats_resize *
 ****************/

void stats_resize() {
  mvwin(stats_win, 6, (COLS - INFO_W));
  wresize(stats_win, LINES - 7, INFO_W);
  wnoutrefresh(stats_win);
}

/*****************
 * stats_refresh *
 *****************/

int stats_refresh() {
  return wnoutrefresh(stats_win);
}

/*****************
 * stats_display *
 *****************/

void stats_display(game_stats_t *Game) {
  float percentage = 0.0;

  percentage = 100 * ((Game->MarkedMines + Game->BadMarkedMines) /
                      (Game->mines * 1.0));

  wcolor_set(stats_win, CLR_STATBAR, NULL);

  stats_clear();

  // Format the time to something nicely readable.
  if (Game->Time >= 86400) {
    mvwprintw(stats_win, 0, 1, "Time: %d:%02d:%02d:%02ds", Game->Time / 86400,
              (Game->Time % 86400) / 3600, (Game->Time % 3600) / 60,
              Game->Time % 60);
  } else if (Game->Time >= 3600) {
    mvwprintw(stats_win, 0, 1, "Time: %d:%02d:%02ds", Game->Time / 3600,
              (Game->Time % 3600) / 60 , Game->Time % 60 );
  } else if (Game->Time >= 60) {
    mvwprintw(stats_win, 0, 1, "Time: %d:%02ds", Game->Time / 60,
              Game->Time % 60 );
  } else {
    mvwprintw(stats_win, 0, 1, "Time: %ds", Game->Time);
  }

  // Display the stats.
  mvwprintw(stats_win, 1, 1, "Loc: %d, %d",
            Game->CursorX + 1, Game->CursorY + 1);
  mvwprintw(stats_win, 2, 1, "Mines: %d", Game->mines);
  mvwprintw(stats_win, 3, 1, "Marks:");
  mvwprintw(stats_win, 4, 1, "Percentage:");

  // We warn the player if they have flagged to many or the incorrect mines.
  if (percentage >= 100.0) {
    if (has_colors() == TRUE) {
      wcolor_set(stats_win, CLR_STATWARN, NULL);
    } else {
      wstandout(stats_win);
    }
    mvwprintw(stats_win, 3, 8, "%d", Game->MarkedMines + Game->BadMarkedMines);
    mvwprintw(stats_win, 4, 13, "%3.1f%%", percentage);
    if (has_colors() == TRUE) {
      wcolor_set(stats_win, CLR_STATBAR, NULL);
    } else {
      wstandend(stats_win);
    }
  } else {
    mvwprintw(stats_win, 3, 8, "%d", Game->MarkedMines + Game->BadMarkedMines);
    mvwprintw(stats_win, 4, 13, "%3.2f%%", percentage);
  }

  // Flag the window to be refreshed.
  wrefresh(stats_win);
  //noutrefresh();
}

void stats_close() {
  delwin(stats_win);
}
