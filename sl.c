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

/*************
 * game_save *
 *************/

void game_save(game_stats_t* game) {
  FILE* fp;
  char *data_path, *filename;
  int len;

  if ((data_path = xdg_data_home()) == NULL) {
    return;
  }

  // Build the filename.
  len = strlen(data_path) + strlen(DFL_SAVE_FILE) + 2;
  filename = (char*)xmalloc(len);
  snprintf(filename, len, "%s/" DFL_SAVE_FILE, data_path);
  free(data_path);

  log_message("Saving the current game.\n -> %s", filename);

  if ((fp = fopen(filename, "w")) == NULL) {
    log_error("Unable to save game :(");
    free(filename);
    return;
  }
  free(filename);

  // Write the game stats.
  fprintf(fp,
          "%d\n%d\n%d\n%u\n"
          "%u\n%u\n"
          "%d\n%d\n%d\n%d\n"
          "%d\n%d\n%d\n"
          "%u\n%u\n%u\n",
          game->height, game->width, game->percent, game->mines,
          game->MarkedMines, game->BadMarkedMines,
          game->alert, game->theme, game->CursorX, game->CursorY,
          game->LargeBoardX, game->LargeBoardY, game->Status,
          game->FocusX, game->FocusY, game->Time);

  // Write the game field.
  fwrite(game->Field,
         (game->height * ((game->width % 2 ? (game->width) + 1 :
                           game->width)) / 2)
         * sizeof(unsigned char), 1, fp);

  fclose(fp);
}

/*************
 * game_load *
 *************/

int game_load(game_stats_t* game) {
  FILE* fp;
  char *data_path, *filename;
  int len;

  if ((data_path = xdg_data_home()) == NULL) {
    return 0;
  }

  len = strlen(data_path) + strlen(DFL_SAVE_FILE) + 2;
  filename = (char*)xmalloc(len);
  snprintf(filename, len, "%s/" DFL_SAVE_FILE, data_path);
  free(data_path);

  if ((fp = fopen(filename, "r")) == NULL) {
    free(filename);
    return 0;
  }
  log_status("Loading game %s", filename);

  // Load the game stats.
  if (fscanf(fp,
             "%d\n%d\n%d\n%u\n"
             "%u\n%u\n"
             "%d\n%d\n%d\n%d\n"
             "%d\n%d\n%d\n"
             "%u\n%u\n%u\n",
             &game->height, &game->width, &game->percent, &game->mines,
             &game->MarkedMines, &game->BadMarkedMines,
             &game->alert, &game->theme, &game->CursorX, &game->CursorY,
             &game->LargeBoardX, &game->LargeBoardY, &game->Status,
             &game->FocusX, &game->FocusY, &game->Time) == 0) {
    fclose(fp);
    free(filename);
    log_error("Unable to read game stats :(");
    return 0;
  }

  game_create_board(game);

  // Load the game field.
  fread(game->Field,
        (game->height * ((game->width % 2 ? (game->width) + 1 :
                          game->width)) / 2)
        * sizeof(unsigned char), 1, fp);
  fclose(fp);

  // Set the game clock as the real clock.
  g_tick = game->Time;

  // Remove the saved file.
  unlink(filename);
  free(filename);

  return 1;
}
