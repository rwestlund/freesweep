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
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.*
 */

#include "sweep.h"
#include <time.h>

/*******************
 * game_save_image *
 *******************/

void game_save_image(game_stats_t* game) {
  int width, height;
  unsigned char value;
  FILE* fp;
  char filename[40];
  time_t now;

  time(&now);
  strftime(filename, 39, "sweep-%F-%R.ppm", localtime(&now));

  if ((fp = fopen(filename, "w")) == NULL) {
    log_error("Unable to save game image to %s", filename);
    return;
  }
  log_status("Saving game image to %s", filename);

  /* dump the stats out */
  fprintf(fp, "P6\n%d\n%d\n255\n", game->width, game->height);

  for (height = 0; height < game->height; height++) {
    for (width = 0; width < game->width; width++) {
      if ((abs(width - game->CursorX) < 2) &&
          (abs(game->CursorY - height) < 10)) {
        fwrite("\xff\x00\x00", 1, 3, fp);

      } else if ((abs(width - game->CursorX) < 10) &&
                 (abs(game->CursorY - height) < 2)) {
        fwrite("\xff\x00\x00", 1, 3, fp);

      } else {
        value = game_get_mine(game, width, height);
        switch ( value ) {
        case UNKNOWN: case MINE:
          fwrite("\x00\x00\x00", 1, 3, fp);
          break;
        case MARKED: case BAD_MARK:
          fwrite("\x00\x00\xff", 1, 3, fp);
          break;
        case DETONATED:
          fwrite("\xff\x00\x00", 1, 3, fp);
          break;
        case EMPTY: default:
          fwrite("\xff\xff\xff", 1, 3, fp);
          break;
        }
      }
    }
  }

  fclose(fp);
}
