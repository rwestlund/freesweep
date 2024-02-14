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
 * game_wipe *
 *************/

static void game_wipe(game_stats_t *game) {
  /* Get the game ready for a reconfigure */
  if (game->percent != 0) {
    game->mines = 0;
  }
  game->MarkedMines = 0;
  game->BadMarkedMines = 0;
  game->LargeBoardX = game->LargeBoardY = 0;
  game->FocusX = game->FocusY = 0;
  game->Time = 0;

  werase(game->Board);
  wnoutrefresh(game->Board);
  werase(game->Border);
  wnoutrefresh(game->Border);
  delwin(game->Board);
  delwin(game->Border);
  free(game->Field);

  game->Board = NULL;
  game->Border = NULL;
  game->Field = NULL;
}

/***************
 * check_width *
 ***************/

int check_width(int value) {
  return ((value >= MIN_W) && (value <= MAX_W));
}

/****************
 * check_height *
 ****************/

int check_height(int value) {
  return ((value >= MIN_H) && (value <= MAX_H));
}

/***************
 * check_theme *
 ***************/

int check_theme(int value) {
  return ((value >= MIN_THEME) && (value <= MAX_THEME));
}

/*****************
 * check_percent *
 *****************/

int check_percent(int value) {
  return ((value >= MIN_PERCENT) && (value <= MAX_PERCENT));
}

/*************
 * game_init *
 *************/

int game_init(game_stats_t* game) {
  game->height = DEFAULT_HEIGHT;
  game->width = DEFAULT_WIDTH;
  game->percent = DEFAULT_PERCENT;
  game->mines = DEFAULT_NUMMINES;
  game->MarkedMines = 0;
  game->BadMarkedMines = 0;
  game->alert = DEFAULT_ALERT;
  game->LargeBoardX = game->LargeBoardY = 0;
  game->FocusX = game->FocusY = 0;
  game->Time = 0;
  game->Status = INPROG;
  game->theme = DEFAULT_THEME;

  game->Board = NULL;
  game->Border = NULL;
  game->Field = NULL;

  theme_set(game);

  return 0;
}

/************
 * game_new *
 ************/

int game_new(game_stats_t* game) {
  WINDOW* win;
  char line[6] = "\0\0\0\0\0";
  int input, field = 1, value;

  if (game_load(game)) {
    return 1;
  }

  if ((win = newwin(LINES - 1, COLS - INFO_W, 0, 0)) == NULL) {
    log_error("Unable to create new game window.");
    return 0;
  }

  game_wipe(game);

  // Draw the initial form.
  mvwprintw(win, 0, (COLS - INFO_W - 18) / 2,
            "New Mission Report");
  mvwhline(win, 1, 1, CharSet.HLine, COLS - INFO_W - 2);

  mvwprintw(win, 3, 2, "Height [10-%d]:", MAX_H);
  mvwprintw(win, 4, 2, " Width [10-%d]:", MAX_W);
  mvwprintw(win, 5, 2, " Mines [10%%-90%%]:");
  mvwprintw(win, 6, 2, "           Alert:");

  mvwprintw(win, 3, 29, "Theme:");

  mvwprintw(win, 8, 2, "Press \'q\' to quit");
  mvwprintw(win, 9, 2, "Press enter to play");

  snprintf(line, 5, "%d", game->height);

  while (field < 6) {
    // Clear the fields
    mvwprintw(win, 3, 19, "       ");
    mvwprintw(win, 4, 19, "       ");
    mvwprintw(win, 5, 19, "       ");
    mvwprintw(win, 6, 19, "       ");
    mvwprintw(win, 3, 35, "            ");
    switch ((field - 1) / 4) {
    case 0:
      mvwprintw(win, 2 + field, 19, ">     <");
      break;
    case 1:
      mvwprintw(win, 2 + (field - 4), 35, ">          <");
      break;
    }

    // Redraw all the fields
    if (field == 1) {
      if (game->height != atoi(line) && has_colors() == TRUE)
        wcolor_set(win, CLR_WARN, NULL);
      mvwprintw(win, 3, 20, "%s", line);
      if (has_colors() == TRUE)
        wcolor_set(win, CLR_NORMAL, NULL);
    } else {
      mvwprintw(win, 3, 20, "%d", game->height);
    }
    if (field == 2) {
      if (game->width != atoi(line) && has_colors() == TRUE)
        wcolor_set(win, CLR_WARN, NULL);
      mvwprintw(win, 4, 20, "%s", line);
      if (has_colors() == TRUE)
        wcolor_set(win, CLR_NORMAL, NULL);
    } else {
      mvwprintw(win, 4, 20, "%d", game->width);
    }
    if (field == 3) {
      if (game->percent != atoi(line) && has_colors() == TRUE)
        wcolor_set(win, CLR_WARN, NULL);
      mvwprintw(win, 5, 20, "%s", line);
      if (has_colors() == TRUE)
        wcolor_set(win, CLR_NORMAL, NULL);
    } else {
      mvwprintw(win, 5, 20, "%d", game->percent);
    }
    mvwprintw(win, 3, 36, "%s", CharSet.name);
    switch (game->alert) {
    case BEEP:
      mvwprintw(win, 6, 20, "Beep");
      break;
    case FLASH:
      mvwprintw(win, 6, 20, "Flash");
      break;
    case NO_ALERT:
      mvwprintw(win, 6, 20, "None");
      break;
    }
    wnoutrefresh(win);

    // Update the cursor
    if (field < 4) {
      curs_set(1);
      wmove(win, 2 + field, 20 + strlen(line));
    } else {
      curs_set(0);
    }
    wrefresh(win);

    cbreak();
    input = getch();
    switch (input) {
    case KEY_ENTER:
    case '\n':
    case ' ':
      field = 6;
      break;
    case 'q':
      config_save(game);
      return 0;
    case KEY_BACKSPACE:
    case '\b':
    case '\x7f':
      if (field < 4 ) {
        if (strlen(line) > 0) {
          line[strlen(line) - 1] = 0;
        }
        switch (field)
          {
          case 1:
            value = atoi(line);
            if (check_height(value)) game->height = value;
            break;
          case 2:
            value = atoi(line);
            if (check_width(value)) game->width = value;
            break;
          case 3:
            value = atoi(line);
            if (check_percent(value)) game->percent = value;
            break;
          }
      }
      break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      if (field < 4) {
        if (strlen(line) < 4) {
          line[strlen(line)] = input;
        }
        switch (field)
          {
          case 1:
            value = atoi(line);
            if (check_height(value)) game->height = value;
            break;
          case 2:
            value = atoi(line);
            if (check_width(value)) game->width = value;
            break;
          case 3:
            value = atoi(line);
            if (check_percent(value) == 1) game->percent = value;
            break;
          }
      }
      break;
    case KEY_RIGHT:
      switch(field) {
      case 1:
        if (game->height < MAX_H) game->height++;
        snprintf(line, 5, "%d", game->height);
        break;
      case 2:
        if (game->width < MAX_W) game->width++;
        snprintf(line, 5, "%d", game->width);
        break;
      case 3:
        if (game->percent < MAX_PERCENT) game->percent++;
        snprintf(line, 5, "%d", game->percent);
        break;
      case 4:
        switch(game->alert)
          {
          case BEEP:
            game->alert = FLASH;
            break;
          case FLASH:
            game->alert = NO_ALERT;
            break;
          case NO_ALERT:
            game->alert = BEEP;
            break;
          }
        break;
      case 5:
        if (game->theme < MAX_THEME) game->theme++;
        else game->theme = MIN_THEME;
        theme_set(game);
        break;
      }
      break;
    case KEY_LEFT:
      switch (field) {
      case 1:
        if (game->height > MIN_H) game->height--;
        snprintf(line, 5, "%d", game->height);
        break;
      case 2:
        if (game->width > MIN_W) game->width--;
        snprintf(line, 5, "%d", game->width);
        break;
      case 3:
        if (game->percent > MIN_PERCENT) game->percent--;
        snprintf(line, 5, "%d", game->percent);
        break;
      case 4:
        switch(game->alert)
          {
          case BEEP:
            game->alert = NO_ALERT;
            break;
          case FLASH:
            game->alert = BEEP;
            break;
          case NO_ALERT:
            game->alert = FLASH;
            break;
          }
        break;
      case 5:
        if (game->theme > MIN_THEME) game->theme--;
        else game->theme = MAX_THEME;
        theme_set(game);
        break;
      }
      break;
    case KEY_DOWN:
      if (field < 5) field++;
      else field = 1;
      switch (field)
        {
        case 1:
          snprintf(line, 5, "%d", game->height);
          break;
        case 2:
          snprintf(line, 5, "%d", game->width);
          break;
        case 3:
          snprintf(line, 5, "%d", game->percent);
          break;
        }
      break;
    case KEY_UP:
      if (field > 1) field--;
      else field = 5;
      switch (field)
        {
        case 1:
          snprintf(line, 5, "%d", game->height);
          break;
        case 2:
          snprintf(line, 5, "%d", game->width);
          break;
        case 3:
          snprintf(line, 5, "%d", game->percent);
          break;
        case 4:
          break;
        }
      break;
    }
  }

  game_create_board(game);
  game_set_mines(game);
  config_save(game);
  curs_set(0);
  wclear(win);
  wnoutrefresh(win);
  delwin(win);
  refresh();
  return TRUE;
}

/*********************
 * game_create_board *
 *********************/

int game_create_board(game_stats_t* game) {
  const int VViewable = (LINES - 3);
  const int HViewable = ((COLS - INFO_W - 2) / 3);

  // Clean up any previous game.
  if (game->Field) {
    free(game->Field);
    game->Field = NULL;
  }
  if (game->Board) {
    delwin(game->Board);
    game->Board = NULL;
  }
  if (game->Border) {
    delwin(game->Border);
    game->Border = NULL;
  }

  // Create the game field.
  if ((game->Field = calloc((game->height * ((game->width % 2 ?
                                              (game->width) + 1 :
                                              game->width))) / 2,
                            sizeof(char))) == NULL) {
    log_error("Unable to allocate game field");
    return 0;
  }

  // Determine if the game field is larger the the terminal area.
  if ((COLS - INFO_W) >= ((3 * game->width) + 2)) {
    game->LargeBoardX = 0;
  } else {
    game->LargeBoardX = 1;
  }

  if ((LINES - 1) >= (game->height + 2)) {
    game->LargeBoardY = 0;
  } else {
    game->LargeBoardY = 1;
  }

  /* Determine the correct size of the border window, and allocate it */
  if (game->LargeBoardX && game->LargeBoardY) {
    game->Border = newwin(VViewable + 2, (3 * HViewable) + 2, 0, 0);
    game->Board  = derwin(game->Border, VViewable, 3 * HViewable, 1, 1);

  } else if (game->LargeBoardX) {
    game->Border = newwin(game->height + 2, (3 * HViewable) + 2, 0, 0);
    game->Board  = derwin(game->Border, game->height, (3 * HViewable), 1, 1);

  } else if (game->LargeBoardY) {
    game->Border = newwin(VViewable + 2, (3 * game->width) + 2, 0, 0);
    game->Board  = derwin(game->Border, VViewable, 3 * game->width, 1, 1);

  } else {
    game->Border = newwin((game->height + 2), (3 * game->width) + 2, 0, 0);
    game->Board  = derwin(game->Border, game->height, 3 * game->width, 1, 1);
  }

  if ((game->Border == NULL) || (game->Board == NULL)) {
    log_error("Unable to allocate game board");
    return 0;
  }

  return 1;
}

/*************
 * game_args *
 *************/

int game_args(game_stats_t* game, int argc, char** argv) {
#if HAVE_GETOPT || HAVE_GETOPT_LONG
  int value = 0, opt = 0;
  int ErrorFlag = 0, BestTimesFlag = 0;
  int PercentFlag = 0, HelpFlag = 0;
  extern int opterr, optind;
  extern char* optarg;

  /* Clear the error flag. */
  opterr = 0;

#if HAVE_GETOPT_LONG
  static struct option long_options[] = {
    {"percent", required_argument, 0, '%'},
    {"show-best-times", no_argument, 0, 'b'},
    {"theme", required_argument, 0, 't'},
    {"height", required_argument, 0, 'h'},
    {"help", no_argument, 0, 'H'},
    {"version", no_argument, 0, 'v'},
    {"width", required_argument, 0, 'w'},
    {0, 0, 0, 0}
  };

  /* Parse the command line options. */
  while ((opt = getopt_long(argc, argv, "%:bh:st:vw:", long_options,
                            NULL)) != EOF) {
#elif HAVE_GETOPT
  /* Parse the command line options. */
  while ((opt = getopt(argc, argv, "%:bh:st:vw:")) != EOF) {
#endif /* HAVE_GETOPT */

    switch (opt) {
    case '%':
      /* Set percent to optarg */
      value = atoi(optarg);
      if (check_percent(value)) {
        game->percent = value;
      } else {
       fprintf(stderr, "Invalid value, %d%%, for percent.\n", value);
       ErrorFlag++;
      }
      PercentFlag++;
      break;
    case 'H':
      /* Show the help listing, but exit without error. */
      break;
    case 'b':
      BestTimesFlag++;
      break;
    case 't':
      /* Set the theme to optarg */
      value = atoi(optarg);
      if (check_theme(value)) {
        game->theme = value;
      } else {
       fprintf(stderr,"Invalid value, %d, for theme.\n", value);
       ErrorFlag++;
      }
      break;
    case 'h':
      /* Set height to optarg */
      value = atoi(optarg);
      if (check_height(value)) {
        game->height = value;
      } else {
        fprintf(stderr, "Invalid value for height.\n");
        ErrorFlag++;
      }
      break;
    case 'v':
      printf("Freesweep %s\n", VERSION);
      exit(EXIT_SUCCESS);
      break;
    case 'w':
      /* Set width to optarg */
      value = atoi(optarg);
      if (check_width(value)) {
        game->width = value;
      } else {
        fprintf(stderr, "Invalid value for width.\n");
        ErrorFlag++;
      }
      break;
    case '?':
    default:
      ErrorFlag++;
      break;
    }
    value = 0;
  }

  /* Make sure there aren't any more arguments. */
  /* Also insure that there was not more than one -s was passed. */

  if (PercentFlag > 1) {
    fprintf(stderr, "Only one value for percentage can be specified.\n");
    ErrorFlag++;
  }

  if (BestTimesFlag > 1) {
    fprintf(stderr, "Only one show best times can be specified.\n");
    ErrorFlag++;
  }

  if (optind != argc) {
    fprintf(stderr, "Non-option arguments are invalid.\n");
    ErrorFlag++;
  }

  if (ErrorFlag + HelpFlag > 0) {
#if defined HAVE_GETOPT_LONG
    fprintf(stderr,"Usage:\n  freesweep [OPTIONS]\n"
            "\t-%% value, --percent=value\tSet percent to value\n"
            "\t-b, --show-best-times\t\tDisplay best times\n"
            "\t-t value, --theme=value\t\tSet theme to value\n"
            "\t-h value, --height=value\tSet height to value\n"
            "\t-H, --help\t\t\tDisplay this help message\n"
            "\t-v, --version\t\t\tDisplay version information\n"
            "\t-w value, --width=value\t\tSet width to value\n");
#else
    fprintf(stderr,"Usage:\n  freesweep [OPTIONS]\n"
            "\t-%% value\tSet percent to value\n"
            "\t-b\t\tDisplay best times\n"
            "\t-t value\t\tSet theme to value\n"
            "\t-H\t\tDisplay this help message\n"
            "\t-h value\tSet height to value\n"
            "\t-v\t\tDisplay version information\n"
            "\t-w value\tSet width to value\n");
#endif
    if (ErrorFlag != 0) {
      exit(EXIT_FAILURE);
    } else {
      exit(EXIT_SUCCESS);
    }
  }

  if (BestTimesFlag == 1) {
    bests_print();
    exit(EXIT_SUCCESS);
  }

#endif /* HAVE_GETOPT || HAVE_GETOPT_LONG */
  return 0;
}

/******************
 * game_set_mines *
 ******************/

int game_set_mines(game_stats_t* game) {
  int MinesToSet = 0, RandX = 0, RandY = 0;
  unsigned char CellVal;

  /* Set all of game->Field to 0 */
  memset(game->Field, 0,
         ((game->height * ((game->width % 2 ?
                            (game->width) + 1 : game->width))) / 2));

  if (game->percent != 0) {
    MinesToSet = (game->percent * game->width * game->height) / 100;
  } else {
    MinesToSet = game->mines;
  }

  game->mines = MinesToSet;

  while (MinesToSet > 0) {
    RandX = rand() % game->width;
    RandY = rand() % game->height;
    //GetMine(RandX, RandY, CellVal);
    CellVal = game_get_mine(game, RandX, RandY);

    /* This is gonna be ugly... */
    if (!((abs((((game->width) / 2) - RandX)) < 2) &&
          (abs((((game->height) / 2) - RandY)) < 2))) {
      if (CellVal != MINE) {
        game_set_mine(game, RandX, RandY, MINE);
        MinesToSet--;
      }
    }
  }

  game->MarkedMines = 0;
  game->BadMarkedMines = 0;

  game->CursorX = game->width / 2;
  game->CursorY = game->height / 2;

  return 0;
}

/*****************
 * game_set_mine *
 *****************/

void game_set_mine(game_stats_t* game, int x, int y, unsigned char value) {
  int index = (x / 2) + y * ((game->width + 1) / 2);

  game->Field[index] =
    (!(x % 2) ?
     ((game->Field[index] & 0x0f) | ((unsigned char)(value) << 4)) :
     ((game->Field[index] & 0xf0) | ((unsigned char)(value) & 0x0f)));
}

/*****************
 * game_get_mine *
 *****************/

unsigned char game_get_mine(game_stats_t* game, int x, int y) {
  int result;
  int index = (x / 2) + y * ((game->width + 1) / 2);

  result = (unsigned char)(!(result = game->Field[index]) ?
                           UNKNOWN :
                           (x % 2 ? result & 0x0f : (result & 0xf0) >> 4));
  return result;
}

/***************
 * game_resize *
 ***************/

void game_resize(game_stats_t* game) {
  const int VViewable = LINES - 3;
  const int HViewable = (COLS - INFO_W - 2) / 3;

  // Determine if the game field is larger the the terminal area.
  if ((COLS - INFO_W) >= ((3 * game->width) + 2)) {
    game->LargeBoardX = 0;
  } else {
    game->LargeBoardX = 1;
  }

  if ((LINES - 1) >= (game->height + 2)) {
    game->LargeBoardY = 0;
  } else {
    game->LargeBoardY = 1;
  }

  /* Determine the correct size of the border window, and allocate it */
  if (game->LargeBoardX && game->LargeBoardY) {
    wresize(game->Border, VViewable + 2, (3 * HViewable) + 2);
    wresize(game->Board, VViewable, 3 * HViewable);
    //game->Border = newwin(VViewable + 2, (3 * HViewable) + 2, 0, 0);
    //game->Board  = derwin(game->Border, VViewable, 3 * HViewable, 1, 1);

  } else if (game->LargeBoardX) {
    wresize(game->Border, game->height + 2, (3 * HViewable) + 2);
    wresize(game->Board, game->height, (3 * HViewable));
    //game->Border = newwin(game->height + 2, (3 * HViewable) + 2, 0, 0);
    //game->Board  = derwin(game->Border, game->height, (3 * HViewable), 1, 1);

  } else if (game->LargeBoardY) {
    wresize(game->Border, VViewable + 2, (3 * game->width) + 2);
    wresize(game->Board, VViewable, 3 * game->width);
    //game->Border = newwin(VViewable + 2, (3 * game->width) + 2, 0, 0);
    //game->Board  = derwin(game->Border, VViewable, 3 * game->width, 1, 1);

  } else {
    wresize(game->Border, game->height + 2, (3 * game->width) + 2);
    wresize(game->Board, game->height, 3 * game->width);
    //game->Border = newwin((game->height + 2), (3 * game->width) + 2, 0, 0);
    //game->Board  = derwin(game->Border, game->height, 3 * game->width, 1, 1);
  }
}

/**************
 * game_close *
 **************/

void game_close(game_stats_t* game) {
  delwin(game->Board);
  delwin(game->Border);
  free(game->Field);
}
