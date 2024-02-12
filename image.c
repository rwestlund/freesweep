/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: image.c,v 1.3 2000-11-07 05:31:26 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"
#include <time.h>

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
