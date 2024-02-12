/*                                                                    -*- c -*-
 * This is a simple game log.
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
#include <stdlib.h>

// The maximum number of lines the fifo will hold.
#define MAX_LINES 1000

static WINDOW *log_win = NULL;
static FILE *log_file = NULL;
static alert_t log_alert = NO_ALERT;

// The message FIFO as a simple linked list.
static unsigned int lines = 0;
static log_t *buffer = NULL;
static log_t *end = NULL;

// If there's a memory we add this static entry to the list.
static log_t out_of_memory = {"No memory avaliable for messages :(", NULL};

/***********
 * log_add *
 ***********/

static void log_add(const char *mesg) {
  /* If there was an out of memory error previously, we clear it out
   * and try again. Who knows, some memory could have been cleared up.
   */
  if (end == &out_of_memory) {
    if (buffer == &out_of_memory) {
      buffer = end = NULL;
      lines = 0;
    } else {
      end = buffer;
      while (end->next != &out_of_memory) end = end->next;
      end->next = NULL;
    }
  }

  // Log the messages.
  if (log_file) {
    fprintf(log_file, "%s\n", mesg);
  }

  if (buffer == NULL) {
    // First message into the fifo.
    if ((buffer = malloc(sizeof(log_t))) == NULL) {
      buffer = end = &out_of_memory;
      return;
    }

    if ((buffer->line = malloc(strlen(mesg) + 1)) == NULL) {
      free(buffer);
      buffer = end = &out_of_memory;
      return;
    }

    buffer->next = NULL;
    strcpy(buffer->line, mesg);

    end = buffer;
    lines = 1;

  } else if (end != &out_of_memory) {
    // Add the new message to the end of the fifo.
    log_t *cur;

    if (lines < MAX_LINES) {
      // Add a new line to the FIFO.
      if ((cur = malloc(sizeof(log_t))) == NULL) {
        end->next = &out_of_memory;
        end = end->next;
        return;
      }
    } else {
      /* The FIFO is full, so lets just reuse the struct that would get
       * discarded.
       */
      cur = buffer;
      buffer = buffer->next;
      free(cur->line);
      lines--;
    }

    if ((cur->line = malloc(strlen(mesg) + 1)) == NULL) {
      free(cur);
      end->next = &out_of_memory;
      end = end->next;
      return;
    }
    cur->next = NULL;
    strcpy(cur->line, mesg);

    end->next = cur;
    end = end->next;
    lines++;
  }
}

/***************
 * log_display *
 ***************/

static void log_display(const char *mesg) {
  // Display the message on the screen.
  if (log_win) {
    wclear(log_win);
    mvwprintw(log_win, 0, 0, mesg);
    wnoutrefresh(log_win);
    wrefresh(log_win);
  }
}

/**************
 * log_splash *
 **************/

void log_splash(int ncols, int y, int x) {
  /* This initializes the logging system and displays logs during the splash
   * screen. log_init will move it to the bottom of the terminal when the
   * starts.
   */
  char *data_path, *log_path;

  if ((log_win = newwin(1, ncols, y, x)) == NULL) {
    return;
  }

  scrollok(log_win, FALSE);
  wclear(log_win);
  wnoutrefresh(log_win);

  if ((data_path = xdg_data_home()) == NULL) {
    return;
  }

  log_path = (char*)xmalloc(MAX_LINE);
  snprintf(log_path, MAX_LINE, "%s/" DFL_LOG_FILE, data_path);
  free(data_path);

  log_file = fopen(log_path, "w");
  free(log_path);
}

/************
 * log_init *
 ************/

void log_init(game_stats_t* game) {
  if (log_win) {
    mvwin(log_win, LINES - 1, 0);
    wresize(log_win, 1, COLS);

    wbkgdset(log_win,  ' ' | COLOR_PAIR(CLR_MSGBAR));
    wclear(log_win);
    wnoutrefresh(log_win);
    wrefresh(log_win);
  }

  log_alert = game->alert;
}

/*****************
 * log_set_alert *
 *****************/

void log_set_alert(game_stats_t* game) {
  log_alert = game->alert;
}

/**************
 * log_resize *
 **************/

void log_resize() {
  if (log_win) {
    mvwin(log_win, LINES - 1, 0);
    wresize(log_win, 1, COLS);
    wnoutrefresh(log_win);
  }
}

/***************
 * log_refresh *
 ***************/

void log_refresh() {
  if (log_win) {
    wnoutrefresh(log_win);
    wrefresh(log_win);
  }
}

/***************
 * log_message *
 ***************/

void log_message(const char *format, ...) {
  static char cooked[MAX_LINE];
  va_list args;

  // Don't add empty messages to the FIFO.
  if (format == NULL || strlen(format) == 0) return;

  // Put the message together.
  va_start(args, format);
  vsnprintf(cooked, MAX_LINE, format, args);
  va_end(args);

  log_add(cooked);
}

/**************
 * log_status *
 **************/

void log_status(const char *format, ...) {
  static char cooked[MAX_LINE];
  va_list args;

  // Don't add empty messages to the FIFO.
  if (format == NULL || strlen(format) == 0) {
    log_display("");
    return;
  }

  // Put the message together.
  va_start(args, format);
  vsnprintf(cooked, MAX_LINE, format, args);
  va_end(args);

  // Display the message on the screen.
  log_display(cooked);
  log_add(cooked);
}

/**************
 * log_error *
 **************/

void log_error(const char *format, ...) {
  static char cooked[MAX_LINE];
  va_list args;

  // Don't add empty messages to the FIFO.
  if (format == NULL || strlen(format) == 0) return;

  // Put the message together.
  va_start(args, format);
  vsnprintf(cooked, MAX_LINE, format, args);
  va_end(args);

  // Display the message on the screen.
  log_display(cooked);
  log_add(cooked);

  switch (log_alert) {
  case NO_ALERT:
    break;
  case FLASH:
    flash();
    break;
  case BEEP: default:
    /* No preference gets a beep. */
    beep();
    break;
  }
}

/********
 * logs *
 ********/

log_t* logs() {
  return buffer;
}

/*************
 * log_close *
 *************/

void log_close() {
  log_t* next;

  // Clean up the log fifo.
  while (buffer) {
    next = buffer->next;

    if (buffer != &out_of_memory) {
      free(buffer->line);
      free(buffer);
    }

    buffer = next;
  }

  // Delete the curses window and close the log file.
  delwin(log_win);
  fclose(log_file);

  // Reset everything
  lines = 0;
  buffer = NULL;
  end = NULL;
  log_win = NULL;
  log_file = NULL;
}
