/*                                                                    -*- c -*-
 * Support for XDG base directory specifications.
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
 *
 * The fifo will no fail if there is a memory error. If there is a memory error
 * a static message will get added to the end of the fifo and any new messages
 * will get dropped. Even after an error occurs new messages will still attempt
 * to be added to the end of the fifo incase system memory was freed.
 */

#include "sweep.h"
#include <string.h>

/*******************
 * xdg_config_home *
 *******************/

char *xdg_config_home() {
  char *home = getenv("HOME");
  char *home_config = getenv("XDG_CONFIG_HOME");

  // $HOME is not set, yikes!
  if (home == NULL) {
    log_error("$HOME not set");
    return NULL;
  }

  if (home_config == NULL) {
    /* XDG_CONFIG_HOME hasn't been set so use the default location and make
     * sure it exists if possible.
     */
    int len = strlen(home) + strlen("/.config/freesweep") + 1;
    char *result = (char*)xmalloc(len);

    // Make sure $HOME/.config exists
    snprintf(result, len, "%s/.config", home);
    if (!xexists(result)) {
      if (mkdir(result, 0750) == -1) {
        log_error("Unable to create the directory $HOME/.config");
        return NULL;
      }
    }

    // Make sure $HOME/.config/freesweep exists
    strncat(result, "/freesweep", MAX_LINE);
    if (!xexists(result)) {
      if (mkdir(result, 0750) == -1) {
        log_error("Unable to create the directory $HOME/.config/freesweep");
        return NULL;
      }
    }

    return result;
  } else {
    /* XDG_CONFIG_HOME has been set. I beleive it is up to the user to ensure
     * this directory exists we only attempt to create the freesweep directory
     * within it.
     */
    int len = strlen(home_config) + strlen("/freesweep") + 1;
    char *result = (char*)xmalloc(len);

    snprintf(result, len, "%s/freesweep", home_config);

    if (!xexists(result)) {
      if (mkdir(result, 0750) == -1) {
        log_error("Unable to create the directory $XDG_CONFIG_HOME/freesweep");
        return NULL;
      }
    }

    return result;
  }
}

/*****************
 * xdg_data_home *
 *****************/

char *xdg_data_home() {
  char *home = getenv("HOME");
  char *home_data = getenv("XDG_DATA_HOME");

  if (home == NULL) {
    log_error("$HOME not set");
    return NULL;
  }

  if (home_data == NULL) {
    /* XDG_DATA_HOME hasn't been set so use the default location and make
     * sure it exists if possible.
     */
    int len = strlen(home) + strlen("/.local/share/freesweep") + 1;
    char *result = (char*)xmalloc(len);

    snprintf(result, len, "%s/.local", home);

    // Make sure $HOME/.local exists
    if (!xexists(result)) {
      if (mkdir(result, 0750) == -1) {
        log_error("Unable to create the directory $HOME/.local");
        return NULL;
      }
    }

    // Make sure $HOME/.local/share exists
    strncat(result, "/share", MAX_LINE);
    if (!xexists(result)) {
      if (mkdir(result, 0750) == -1) {
        log_error("Unable to create the directory $HOME/.local/share");
        return NULL;
      }
    }

    // Make sure $HOME/.local/share/freesweep exists
    strncat(result, "/freesweep", MAX_LINE);
    if (!xexists(result)) {
      if (mkdir(result, 0750) == -1) {
        log_error("Unable to create the directory "
                  "$HOME/.local/share/freesweep");
        return NULL;
      }
    }

    return result;
  } else {
    /* XDG_DATA_HOME has been set. I beleive it is up to the user to ensure
     * this directory exists we only attempt to create the freesweep directory
     * within it.
     */
    int len = strlen(home_data) + strlen("/freesweep") + 1;
    char *result = (char*)xmalloc(len);

    snprintf(result, len, "%s/freesweep", home_data);

    if (!xexists(result)) {
      if (mkdir(result, 0750) == -1) {
        log_error("Unable to create the directory $XDG_DATA_HOME/freesweep");
        return NULL;
      }
    }

    return result;
  }
}
