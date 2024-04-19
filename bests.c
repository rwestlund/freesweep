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
#include <pwd.h>

/*************
 * table_new *
 *************/

static bests_table_t* table_new() {
  bests_table_t* table = NULL;

  table = (bests_table_t*)xmalloc(sizeof(bests_table_t));

  table->entries = NULL;
  table->numents = 0;
  table->alloced = 0;
  table->sorted = TRUE;

  return table;
}

/****************
 * entry_insert *
 ****************/

static void entry_insert(bests_table_t* table, bests_entry_t* entry) {
  int i;

  // Search until I find a match.
  for (i = 0; i < table->numents; i++) {
    if (entry->user == table->entries[i].user &&
        entry->area == table->entries[i].area &&
        entry->mines == table->entries[i].mines) {

      // Replace it and mark the flag.
      if (entry->time < table->entries[i].time) {
        // Only update if the time is better.
        table->entries[i].time = entry->time;
        table->entries[i].date = entry->date;
      }

      table->sorted = FALSE;
      return;
    }
  }

  // Allocate more space, 5 more entries, if it is necessary.
  if (table->alloced == 0) {
    table->entries = xmalloc(sizeof(bests_entry_t) * 5);
    table->alloced += 5;
  } else if (table->numents >= table->alloced) {
    bests_entry_t* orig = table->entries;
    table->entries = xmalloc(sizeof(bests_entry_t) *
                             (table->alloced + 5));

    memcpy(table->entries, orig, table->alloced);
    free(orig);
    table->alloced += 5;
  }

  // Add the new entry.
  table->entries[table->numents].area = entry->area;
  table->entries[table->numents].mines = entry->mines;
  table->entries[table->numents].time = entry->time;
  table->entries[table->numents].user = entry->user;
  table->entries[table->numents].date = entry->date;
  table->numents++;

  table->sorted = FALSE;
}

/*************
 * entry_cmp *
 *************/

static int entry_cmp(const void* l, const void* r) {
  bests_entry_t* bl = (bests_entry_t*)l;
  bests_entry_t* br = (bests_entry_t*)r;

  if (bl->area < br->area) {
    return 1;
  } else if (bl->area > br->area) {
    return -1;
  } else if (bl->area == br->area) {
    if (bl->mines < br->mines) {
      return 1;
    } else if (bl->mines > br->mines) {
      return -1;
    } else if (bl->mines == br->mines) {
      if (bl->time < br->time) {
        return 1;
      } else if (bl->time > br->time) {
        return -1;
      }	else if (bl->time == br->time) {
        return 0;
      }
    }
  }

  // Shut the compiler up this never happens.
  return 0;
}

/**************
 * table_sort *
 **************/

static void table_sort(bests_table_t* table) {
  if (table->sorted == TRUE) {
    // The list is already sorted, so there's nothing to do.
    return;
  } else {
    // qsort the whole mess.
    log_message("Sorting the best times.");
    qsort(table->entries, table->numents, sizeof(table->entries[0]),
          entry_cmp);
    log_message("  %d entries sorted.", table->numents);
    table->sorted = TRUE;
  }
}

/*******************
 * bests_user_file *
 *******************/

static char* bests_user_file(void) {
  char *xdg_home = NULL;
  char *filename = NULL;

  if ((xdg_home = xdg_data_home()) == NULL) {
    return NULL;
  }

  /* get me some memory for the string */
  filename = (char*)xmalloc(MAX_LINE);

  /* make the full path */
  snprintf(filename, MAX_LINE, "%s/" DFL_BESTS_FILE, xdg_home);
  free(xdg_home);

  return filename;
}

/*********************
 * bests_global_file *
 *********************/

static inline char* bests_global_file(void) {
  return mkstr2(LOCALSTATEDIR) "/sweeptimes";
}

/**********
 * tlockf *
 **********/

static void tlockf(FILE *fp, char *name) {
  int fd;

  /* this is actually dark magic. You aren't supposed to mix fileno
   * and streams */
  fflush(fp);

#if defined(HAVE_FILENO)
  fd = fileno(fp);
#else
#error "Need fileno() replacement"
#endif

  lseek(fd, 0L, SEEK_SET);

#if defined(HAVE_FLOCK) && defined(HAVE_LOCKF)
  if(flock(fd, LOCK_EX) == -1) {
#elif defined(HAVE_FLOCK)
  if(flock(fd, LOCK_EX) == -1) {
#elif defined(HAVE_LOCKF)
  if(lockf(fd, F_LOCK, 0L) == -1) {
#else
#error "Need flock() or lockf()"
#endif
    log_message("Cannot lock file %s", name);
    exit(EXIT_FAILURE);
  }
}

/************
 * tunlockf *
 ************/

static void tunlockf(FILE* fp) {
  int fd;

  /* This is actually dark magic. You aren't supposed to mix fileno and
   * streams.
   */
  fflush(fp);

#if defined(HAVE_FILENO)
  fd = fileno(fp);
#else
#error "Need fileno() replacement"
#endif

  lseek(fd, 0L, SEEK_SET);

#if defined(HAVE_FLOCK) && defined(HAVE_LOCKF)
  if (flock(fd, LOCK_UN) == -1) {
#elif defined(HAVE_FLOCK)
  if (flock(fd, LOCK_UN) == -1) {
#elif defined(HAVE_LOCKF)
  if (lockf(fd, F_ULOCK, 0L) == -1) {
#else
#error "Need flock() or lockf()"
#endif
    log_message("Can't unlock file");
    exit(EXIT_FAILURE);
  }
}

/*******************
 * bests_save_file *
 *******************/

static void bests_save_file(bests_table_t* table, char *filename,
                            int useronly) {
  FILE *fp;
  bests_entry_t *entry;
  int i;

  if ((fp = fopen(filename, "w")) != NULL) {
    log_message("Writing best times file %s.", filename);

    tlockf(fp, filename);

    for (i = 0; i < table->numents; i++) {
      entry = &table->entries[i];
      if (!useronly || entry->user == getuid()) {
        fprintf(fp, "%u,%u,%u,%u,%u\n",
                entry->user, entry->area, entry->mines, entry->time,
                (unsigned int)entry->date);
      }
    }

    tunlockf(fp);
    fclose(fp);
  }
}

/*******************
 * bests_load_file *
 *******************/

static void bests_load_file(bests_table_t* table, char* filename) {
  FILE *fp;
  int result;

  log_message("Reading best times file %s.", filename);

  if ((fp = fopen(filename, "r")) != NULL) {
    bests_entry_t entry;

    tlockf(fp, filename);

    while (!feof(fp)) {
      if ((result = fscanf(fp, "%u,%u,%u,%u,%lu",
                           &entry.user, &entry.area, &entry.mines,
                           &entry.time, (unsigned long *)&entry.date)) != 5) {

        // Return value of -1 is from the end of file.
        if (result != -1) {
          log_error("Error reading line from records file.");
          log_error(" Read %d columns instead of 5.", result);
        }
      } else {
        entry_insert(table, &entry);
      }
    }

    tunlockf(fp);
    fclose(fp);	/* you just try! */
  }

  table_sort(table);
}

/****************
 * bests_update *
 ****************/

void bests_update(game_stats_t* game) {
  if (game->theme != 9) {
    char* pathname = bests_user_file();
    bests_table_t* table = bests_load();
    bests_entry_t entry;

    if (!pathname) return;

    entry.area = game->height * game->width;
    entry.mines = game->mines;
    entry.time = game->Time;
    entry.user = getuid();
    entry.date = time(NULL);

    entry_insert(table, &entry);
    table_sort(table);

    bests_save_file(table, pathname, TRUE);
    free(pathname);

    bests_save_file(table, bests_global_file(), FALSE);

    bests_free(table);
  }
}

/**************
 * bests_load *
 **************/

bests_table_t* bests_load() {
  bests_table_t* table = table_new();
  char* pathname = bests_user_file();

  if (pathname) {
    bests_load_file(table, pathname);
    free(pathname);
  }

  bests_load_file(table, bests_global_file());

  return table;
}

/**************
 * bests_free *
 **************/

void bests_free(bests_table_t* table) {
  if (table) {
    free(table->entries);
    free(table);
  }
}

/***************
 * bests_print *
 ***************/

void bests_print() {
  bests_table_t* table = bests_load();
  int i;
  time_t enttime;

  printf("Player\tArea\tMines\tTime\tDate\n");

  for (i = 0; i < table->numents; i++) {
    const struct passwd *pw = getpwuid(table->entries[i].user);

    printf("%s\t%u\t%u\t", pw->pw_name, table->entries[i].area,
           table->entries[i].mines);

    enttime = table->entries[i].time;
    if (enttime >= 86400) {
      printf("%ld:%02ld:%02ld:%02lds",
             enttime / 86400,
             (enttime % 86400) / 3600,
             (enttime % 3600) / 60 ,
             enttime % 60);
    } else if (enttime >= 3600) {
      printf("%ld:%02ld:%02lds", enttime / 3600, (enttime % 3600) / 60,
             enttime % 60);
    } else if (enttime >= 60) {
      printf("%ld:%02lds", enttime / 60, enttime % 60);
    } else {
      printf("%lds", enttime);
    }
    printf("\t%s", ctime(&table->entries[i].date));
  }
  bests_free(table);
}
