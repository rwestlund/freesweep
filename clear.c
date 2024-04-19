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

/* This is the struct for the clearing algo. */
typedef struct mark_s {
  int x, y;
  struct mark_s *next;
} mark_t;

static int g_num = 0;
static unsigned char *g_table = NULL;
static int g_table_w;
static int g_table_h = 0;
static mark_t *g_mlist = NULL;

static inline unsigned char LOOKUP(unsigned char *t, int xx, int yy) {
  return (unsigned char)((t)[(xx) / 8 + yy * g_table_w]) &
    (((unsigned char)0x80) >> ((xx) % 8));
}

static inline void SET(unsigned char *t, int xx, int yy) {
  ((t)[(xx) / 8 + yy * g_table_w]) |=
    (((unsigned char)0x80) >> ((xx) % 8));
}

static inline void UNSET(unsigned char *t, int xx, int yy)  {
  ((t)[(xx)/8 + yy * g_table_w]) &=
    ~(((unsigned char)0x80) >> ((xx) % 8));
}

/************
 * mark_new *
 ************/

static mark_t* mark_new(int x, int y) {
  /* make a new mark initialized nicely */
  mark_t *m = NULL;

  m = (mark_t*)malloc(sizeof(mark_t) * 1);

  if (m == NULL) {
    log_error("Out of memory! Sorry.");
    endwin();
    exit(EXIT_FAILURE);
  }

  m->x = x;
  m->y = y;
  m->next = NULL;

  return (m);
}

/******************
 * ht_mark_insert *
 ******************/

static void ht_mark_insert(int x, int y) {
  /* if x,y exists, do nothing, otherwise, make a mark and insert it */
  mark_t *m = NULL;

  if (LOOKUP(g_table, x, y)) {
    return;
  }
  SET(g_table, x, y);
  g_num++;

  m = mark_new(x, y);

  m->next = g_mlist;
  g_mlist = m;
}

/********************
 * ht_delete_random *
 ********************/

static char ht_delete_random(int *x, int *y) {
  /* find one in the table and deal with it */
  mark_t *m = NULL;

  m = g_mlist;
  if (m == NULL) {
    return FALSE;
  }

  g_mlist = g_mlist->next;

  *x = m->x;
  *y = m->y;
  UNSET(g_table, *x, *y);

  free(m);

  return TRUE;
}

/***********
 * ht_init *
 ***********/

static unsigned char* ht_init(void) {
  int width, height;
  unsigned char *m = NULL;

  width = MAX_W / 8;
  if (MAX_W % 8) {
    width++;
  }

  height = MAX_H;
  if (MAX_W % 8) {
    height++;
  }

  m = (unsigned char*)malloc(sizeof(unsigned char) * width * height);

  if (m == NULL) {
    log_error("Out of memory. Aborting...");
    endwin();
    exit(EXIT_FAILURE);
  }

  g_table_w = width;
  g_table_h = height;

  memset(m, 0, width * height);

  return (m);
}

/**********************
 * ht_insert_children *
 **********************/

static void ht_insert_children(game_stats_t *game, int x, int y) {
  /* Grab the eight or so children around the mark and insert them into the
   * hash table.
   */
  unsigned char retval;

  /* Check the above squares */
  if (y - 1 >= 0) {
    /* Check upper-left */
    if (x - 1 >= 0) {
      retval = game_get_mine(game, x - 1, y - 1);
      if (retval == UNKNOWN) {
        ht_mark_insert(x-1, y-1);
      }
    }

    /* Check directly above */
    retval = game_get_mine(game, x, y - 1);
    if (retval == UNKNOWN) {
      ht_mark_insert(x, y-1);
    }

    /* Check upper-right */
    if (x + 1 < game->width) {
      retval = game_get_mine(game, x + 1, y - 1);
      if (retval == UNKNOWN) {
        ht_mark_insert(x + 1, y - 1);
      }
    }
  }

  if (x - 1 >= 0) {
    retval = game_get_mine(game, x - 1, y);
    if (retval == UNKNOWN) {
      ht_mark_insert(x - 1, y);
    }
  }

  if (x + 1 < game->width) {
    retval = game_get_mine(game, x + 1, y);
    if (retval == UNKNOWN) {
      ht_mark_insert(x + 1, y);
    }
  }

  /* Check the below squares */
  if (y + 1 < game->height) {
    /* Check lower-left */
    if (x - 1 >= 0) {
      retval = game_get_mine(game, x - 1, y + 1);
      if (retval == UNKNOWN) {
        ht_mark_insert(x - 1, y + 1);
      }
    }

    /* Check directly below */
    retval = game_get_mine(game, x, y + 1);
    if (retval == UNKNOWN) {
      ht_mark_insert(x, y + 1);
    }

    /* Check lower-right */
    if (x + 1 < game->width) {
      retval = game_get_mine(game, x + 1, y + 1);
      if (retval == UNKNOWN) {
        ht_mark_insert(x + 1, y + 1);
      }
    }
  }
}

/*****************
 * square_expose *
 *****************/

static int square_expose(game_stats_t *game, int x, int y, int total) {
  /* fill in what a square is number wise */
  game_set_mine(game, x, y, total == 0 ? EMPTY : total);
  return (total == 0 ? EMPTY : total);
}

/*********************
 * square_mine_count *
 *********************/

static int square_mine_count(game_stats_t *game, int x, int y) {
  /* determine the number of a particular square */
  unsigned char SquareVal;
  int Total = 0;

  SquareVal = game_get_mine(game, x, y);
  if (SquareVal != UNKNOWN) {
    return SquareVal;
  }

  /* Check the above squares */
  if (y - 1 >= 0) {
    /* Check upper-left */
    if (x - 1 >= 0) {
      SquareVal = game_get_mine(game, x - 1, y - 1);
      if ((SquareVal == MINE) ||
          (SquareVal == MARKED) ||
          (SquareVal == DETONATED)) {
        Total++;
      }
    }

    /* Check directly above */
    SquareVal = game_get_mine(game, x, y - 1);
    if ((SquareVal == MINE) ||
        (SquareVal == MARKED) ||
        (SquareVal == DETONATED)) {
      Total++;
    }

      /* Check upper-right */
    if (x + 1 < game->width) {
      SquareVal = game_get_mine(game, x + 1, y - 1);
      if ((SquareVal == MINE) ||
          (SquareVal == MARKED) ||
          (SquareVal == DETONATED)) {
        Total++;
      }
    }
  }

  if (x - 1 >= 0) {
    SquareVal = game_get_mine(game, x - 1, y);
    if ((SquareVal == MINE) ||
        (SquareVal == MARKED) ||
        (SquareVal == DETONATED)) {
      Total++;
    }
  }

  if (x + 1 < game->width) {
    SquareVal = game_get_mine(game, x + 1, y);
    if ((SquareVal == MINE) ||
        (SquareVal == MARKED) ||
        (SquareVal == DETONATED)) {
      Total++;
    }
  }

  /* Check the below squares */
  if (y + 1 < game->height) {
    /* Check lower-left */
    if (x - 1 >= 0) {
      SquareVal = game_get_mine(game, x - 1, y + 1);
      if ((SquareVal == MINE) ||
          (SquareVal == MARKED) ||
          (SquareVal == DETONATED)) {
        Total++;
      }
    }

    /* Check directly below */
    SquareVal = game_get_mine(game, x, y + 1);
    if ((SquareVal == MINE) ||
        (SquareVal == MARKED) ||
        (SquareVal == DETONATED)) {
      Total++;
    }

    /* Check lower-right */
    if (x + 1 < game->width) {
      SquareVal = game_get_mine(game, x + 1, y + 1);
      if ((SquareVal == MINE) ||
          (SquareVal == MARKED) ||
          (SquareVal == DETONATED)) {
        Total++;
      }
    }
  }

  return Total;
}

/**********************
 * square_super_count *
 **********************/

static int square_super_count(game_stats_t* game) {
  /* calculate if I should superclick, die, or do nothing. */
  unsigned char retval;
  int MinesFound = 0, BadFound = 0;
  int x, y;

  x = game->CursorX;
  y = game->CursorY;

  /* Check the above squares */
  if (y - 1 >= 0) {
    /* Check upper-left */
    if (x - 1 >= 0) {
      retval = game_get_mine(game, x - 1, y - 1);
      switch (retval) {
      case MINE:
        MinesFound++;
        break;
      case BAD_MARK:
        BadFound++;
        break;
      default:
        break;
      }
    }

    /* Check directly above */
    retval = game_get_mine(game, x, y - 1);
    switch (retval) {
    case MINE:
      MinesFound++;
      break;
    case BAD_MARK:
      BadFound++;
      break;
    default:
      break;
    }

    /* Check upper-right */
    if (x + 1 < game->width) {
      retval = game_get_mine(game, x + 1, y - 1);
      switch (retval) {
      case MINE:
        MinesFound++;
        break;
      case BAD_MARK:
        BadFound++;
        break;
      default:
        break;
      }
    }
  }

  if (x - 1 >= 0) {
    retval = game_get_mine(game, x - 1, y);
    switch (retval) {
    case MINE:
      MinesFound++;
      break;
    case BAD_MARK:
      BadFound++;
      break;
    default:
      break;
    }
  }

  if (x + 1 < game->width) {
    retval = game_get_mine(game, x + 1, y);
    switch (retval) {
    case MINE:
      MinesFound++;
      break;
    case BAD_MARK:
      BadFound++;
      break;
    default:
      break;
    }
  }

  /* Check the below squares */
  if (y + 1 < game->height) {
    /* Check lower-left */
    if (x - 1 >= 0) {
      retval = game_get_mine(game, x - 1, y + 1);
      switch (retval) {
      case MINE:
        MinesFound++;
        break;
      case BAD_MARK:
        BadFound++;
        break;
      default:
        break;
      }
    }

    /* Check directly below */
    retval = game_get_mine(game, x, y + 1);
    switch (retval) {
    case MINE:
      MinesFound++;
      break;
    case BAD_MARK:
      BadFound++;
      break;
    default:
      break;
    }

    /* Check lower-right */
    if (x + 1 < game->width) {
      retval = game_get_mine(game, x + 1, y + 1);
      switch (retval) {
      case MINE:
        MinesFound++;
        break;
      case BAD_MARK:
        BadFound++;
        break;
      default:
        break;
      }
    }
  }

  if (BadFound != 0) {
    /* some incorrectly marked mines */
    return DIE;
  } else if (MinesFound != 0 && BadFound == 0) {
    /* some correctly marked, some not marked mines */
    return DONOTHING;
  } else {
    /* all mines correctly marked */
    return SUPERCLICK;
  }
}

/**************
 * game_clear *
 **************/

void game_clear(game_stats_t *game) {
  int total;
  int x, y;

  g_num = 0;
  g_table = ht_init();

  /* give it the first candidate */
  ht_mark_insert(game->CursorX, game->CursorY);

  /* This is data recursion */
  while(ht_delete_random(&x, &y) == TRUE) {
    total = square_mine_count(game, x, y);
    if (square_expose(game, x, y, total) == EMPTY) {
      ht_insert_children(game, x, y);
    }
  }

  free (g_table);
}

/********************
 * game_super_clear *
 ********************/

void game_super_clear(game_stats_t *game) {
  int val;
  int total;
  int x, y;

  x = game->CursorX;
  y = game->CursorY;
  val = square_super_count(game);

  switch(val) {
    /* they marked incorrectly, and must now pay the price */
  case DIE:
    //dialog_boom();
    game->Status = LOSE;
    game_set_mine(game, x, y, DETONATED);
    break;
    /* they marked correctly, and I can expand stuff */
  case SUPERCLICK:
    g_num = 0;
    g_table = ht_init();

    /* insert all the ones around me */
    ht_insert_children(game, x, y);

    /* This is data recursion */
    while(ht_delete_random(&x, &y) == TRUE) {
      total = square_mine_count(game, x, y);
      if (square_expose(game, x, y, total) == EMPTY) {
        ht_insert_children(game, x, y);
      }
    }

    free (g_table);
    break;
  case DONOTHING:
    /* yup, just that */
    break;
  default:
    break;
  }
}

/*********************
 * game_find_nearest *
 *********************/

int game_find_nearest(game_stats_t *game) {
  int x, y;
  int minx, miny, minscore, score;
  unsigned char square;

  x = game->CursorX;
  y = game->CursorY;

  square = game_get_mine(game, x, y);
  if (( square == MINE ) || ( square == UNKNOWN )) {
    /* We're *on* an unmarked square already! */
    return 0;
  }

  /* This should be really, really big. */
  minscore = 0x7ffffff;
  minx = game->CursorX;
  miny = game->CursorY;

  for (x = 0; x < game->width; x++) {
    for (y = 0; y < game->height; y++) {
      square = game_get_mine(game, x, y);

      if ((square == MINE) || (square == UNKNOWN)) {
        score = ((game->CursorX - x) * (game->CursorX - x)) +
          ((game->CursorY - y) * (game->CursorY - y));

        if (score < minscore) {
          minscore = score;
          minx = x;
          miny = y;
        }
      }
    }
  }

  game->CursorX = minx;
  game->CursorY = miny;

  log_message("Minimum score of %d found at %d,%d\n", minscore, minx, miny);
  return 0;
}

/*************************
 * game_find_nearest_bad *
 *************************/

int game_find_nearest_bad(game_stats_t *game) {
  int x, y;
  int minx, miny, minscore, score;
  unsigned char square;

  x = game->CursorX;
  y = game->CursorY;

  square = game_get_mine(game, x, y);
  if ((square == BAD_MARK) || (game->BadMarkedMines == 0)) {
    /* We're done here. */
    return 0;
  }

  /* This should be really, really big. */
  minscore = 0x7ffffff;
  minx = game->CursorX;
  miny = game->CursorY;

  for (x = 0; x < game->width; x++) {
    for (y = 0; y < game->height; y++) {
      square = game_get_mine(game, x, y);

      if (square == BAD_MARK) {
        score = ((game->CursorX - x) * (game->CursorX - x)) +
          ((game->CursorY - y) * (game->CursorY - y));

        if (score < minscore) {
          minscore = score;
          minx = x;
          miny = y;
        }
      }
    }
  }

  game->CursorX = minx;
  game->CursorY = miny;

  log_message("Bad mark: minimum score of %d found at %d,%d\n", minscore,
              minx, miny);
  return 0;
}
