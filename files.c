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

/***************
 * config_load *
 ***************/

static int config_load(game_stats_t* game, FILE* PrefsFile, char overwrite) {
  char buffer[MAX_LINE];
  char *key, *value;
  int i_value;

  while (feof(PrefsFile) == 0) {
    if (fgets(buffer, MAX_LINE, PrefsFile) == 0) {
      return 0;
    }
    if ((key = strtok(buffer, DELIMITERS)) != 0) {
      // This is essentially a great big switch statement.
      if ((value = strtok(NULL, DELIMITERS)) != 0) {

        if (strncasecmp(key, "height", 6) == 0) {
          if (overwrite || game->height == DEFAULT_HEIGHT) {
            i_value = atoi(value);
            (check_height(i_value) ?
             game->height = i_value :
             log_error("Invalid value, %d, for height in preference file.",
                       i_value));
          }
        } else if (strncasecmp(key, "theme", 5) == 0) {
          i_value = atoi(value);
          (check_theme(i_value) ?
           game->theme = i_value :
           log_error("Invalid value, %d, for theme in preference file.",
                     i_value));

        } else if (strncasecmp(key, "percent", 7) == 0) {
          i_value = atoi(value);
          (check_percent(i_value) ?
           game->percent = i_value, game->mines = 0 :
           log_error("Invalid value, %d%% for percent in preference file.",
                     i_value));

        } else if (strncasecmp(key, "width", 5) == 0) {
          i_value = atoi(value);
          (check_width(i_value) ?
           game->width = i_value :
           log_error("Invalid value, %d, for width in preference file.",
                     i_value));

        } else if (strncasecmp(key, "alert", 5) == 0) {
          // This option takes different strings as arguments.
          if (strncasecmp(value, "beep", 4) == 0) {
              game->alert = BEEP;
          } else if (strncasecmp(value, "flash", 5) == 0) {
            game->alert = FLASH;
          } else if (strncasecmp(value, "none", 4) == 0) {
            game->alert = NO_ALERT;
          } else {
            log_error("Bad value, %s, for Alert in preference file.", value);
          }
        } else if (strncasecmp(key, "#", 1) != 0) {
          log_error("Unknown tag %s in file.\n", key);
        }
      }
    }
  }
  return 0;
}

/********************
 * config_load_user *
 ********************/

int config_load_user(game_stats_t* game) {
  FILE* fp;
  char *config_path, *filename;
  int len;

  if ((config_path = xdg_config_home()) == NULL) {
    return 1;
  }

  len = strlen(config_path) + strlen(DFL_PREFS_FILE) + 2;
  filename = (char*)xmalloc(len);
  snprintf(filename, len, "%s/" DFL_PREFS_FILE, config_path);
  free(config_path);

  log_status("Loading %s.", filename);

  if ((fp = fopen(filename, "r")) == NULL) {
    log_message("No configuration found.");

    // The user has no personal preferences.
    free(filename);
    return 0;
  } else if (config_load(game, fp, FALSE) == 1) {
    log_error(" Error reading configuration file.");

    // An error occurred while reading the file. Try closing it.
    fclose(fp);
    free(filename);
    return 1;
  } else {
    // Done, so close the file.
    fclose(fp);
    free(filename);
    return 0;
  }
}

/**********************
 * config_load_global *
 **********************/

int config_load_global(game_stats_t* game) {
  FILE* fp;

  log_status("Loading " mkstr2(SYSCONFDIR) "/sweeprc");

  if ((fp = fopen(mkstr2(SYSCONFDIR) "/sweeprc", "r")) == NULL) {
    // The global file is invalid or non-existant.
    log_message(" Unable to open the file");
    return 1;
  } else if (config_load(game, fp, FALSE) == 1) {
    // An error occurred while reading the file. Try closing it.
    fclose(fp);
    return 1;
  } else {
    // Done, so close the file.
    fclose(fp);
    return 0;
  }
}

/***************
 * config_save *
 ***************/

int config_save(game_stats_t* game) {
  FILE* fp;
  char *config_path, *filename;
  int len;

  // Get the XDG specified config directory.
  config_path = xdg_config_home();
  if ((config_path = xdg_config_home()) == NULL) {
    return 1;
  }

  // Build the filename.
  len = strlen(config_path) + strlen(DFL_PREFS_FILE) + 2;
  filename = (char*)xmalloc(len);
  snprintf(filename, len, "%s/" DFL_PREFS_FILE, config_path);
  free(config_path);

  if ((fp = fopen(filename, "w")) == NULL) {
    log_error("Unable to write to prefs file %s", filename);
    free(filename);
    return 1;

  } else {
    // Write to the configuration file.
    fprintf(fp, "# Freesweep version %s\n", VERSION);
    fprintf(fp, "#   This is a generated file and will be overwritten\n");
    fprintf(fp, "#   by the game.\n");
    fprintf(fp, "Height=%d\n", game->height);
    fprintf(fp, "Width=%d\n", game->width);
    fprintf(fp, "Percent=%d\n", game->percent);
    fprintf(fp, "Theme=%d\n", game->theme);
    switch (game->alert) {
    case BEEP:
      fprintf(fp, "Alert=Beep\n");
      break;
    case FLASH:
      fprintf(fp, "Alert=Flash\n");
      break;
    case NO_ALERT:
      fprintf(fp, "Alert=None\n");
      break;
    default:
      break;
    }

    fclose(fp);
  }

  free(filename);
  return 0;
}
