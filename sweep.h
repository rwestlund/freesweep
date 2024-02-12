/*********************************************************************
* $Id: sweep.h.in,v 1.36 2002-12-21 21:32:09 hartmann Exp $
*********************************************************************/

#ifndef __SWEEP_H__
#define __SWEEP_H__

#define mkstr2(s) mkstr(s)
#define mkstr(s) # s

/* The library header files. */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif /* HAVE_GETOPT_H */

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif /* HAVE_ERRNO_H */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <signal.h>

#include <curses.h>

#if defined HAVE_FLOCK && defined HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif

/* XXX make this work with configure */
#include <sys/types.h>
#include <dirent.h>

#ifdef NCURSES_MOUSE_VERSION
#define SWEEP_MOUSE
#endif /* NCURSES_MOUSE_VERSION */

#if defined SCORESDIR
#define USE_GROUP_BEST_FILE
#endif

#include "defaults.h"

/* These are all the defines used in freesweep */
/* These are defines for convenience */
#define DELIMITERS " \t="

#define DFL_PREFS_FILE "config"
#define DFL_BESTS_FILE "records"
#define DFL_SAVE_FILE  "saved"
#define DFL_LOG_FILE   "log"

/* used for the superclick feature in the game */
#define SUPERCLICK 0
#define DIE        1
#define DONOTHING  2

/* These are defines for maximum accepted values */
#define MAX_LINE    1024
#define MAX_H       2048
#define MAX_W       2048
#define MIN_H       10
#define MIN_W       10
#define MAX_PERCENT 90
#define MIN_PERCENT 10
#define MAX_THEME   9
#define MIN_THEME   1
#define INFO_W      21

/* These are the macros for cell values. */
#define UNKNOWN   0x0
#define MINE      0x9
#define MARKED    0xa
#define BAD_MARK  0xb
#define EMPTY     0xc
#define DETONATED 0xd

/* These are for winning and losing. */
typedef enum {
  INPROG = 0,
  WIN    = 1,
  LOSE   = 2,
  ABORT  = 3,
  QUIT   = 4
} game_status_t;

/* These are for the alert types. */
typedef enum {
  NO_ALERT = 0,
  BEEP     = 1,
  FLASH    = 2
} alert_t;

/* These are for color pairs. */
#define CLR_NORMAL 1
#define CLR_VAL1 2
#define CLR_VAL2 3
#define CLR_VAL3 4
#define CLR_VAL4 5
#define CLR_VAL5 6
#define CLR_VAL6 7
#define CLR_VAL7 8
#define CLR_VAL8 9
#define CLR_UNKNOWN 10
#define CLR_MINE 11
#define CLR_SHOWMINE 12
#define CLR_SPACE 13
#define CLR_MARK 14
#define CLR_FALSEMARK 15
#define CLR_SHOWFALSEMARK 16
#define CLR_DETONATED 17
#define CLR_INFOBAR 18
#define CLR_STATBAR 19
#define CLR_STATWARN 20
#define CLR_MSGBAR 21
#define CLR_WARN 22

/* This extends the naming convention of ncurses functions. */
#define mvclrtoeol(y,x) (move(y,x)==ERR?ERR:clrtoeol());
#define noutrefresh() wnoutrefresh(stdscr)

#ifndef mvwhline
#define mvwhline(w,y,x,z,n) (wmove(w,y,x)==ERR?ERR:whline(w,z,n));
#endif /* mvwhline */

#ifndef mvwvline
#define mvwvline(w,y,x,z,n) (wmove(w,y,x)==ERR?ERR:wvline(w,z,n));
#endif /* mvwvline */

#ifndef mvgetnstr
#define mvgetnstr(y, x, str, n) (wmove(stdscr, y, x)==ERR?ERR:wgetnstr(stdscr, str, n));
#endif /* mvgetnstr */

/* These are all the structs used in freesweep */

/* Single byte or Unicode characters. */
typedef union _ch_type {
  chtype ch;
  char* str;
} ch_type;

/* This is the struct containing all the various drawing characters. */
typedef struct _DrawChars {
  enum { CH_TYPE, UNI_TYPE } ch_flag;
  char* name;
  chtype ULCorner;
  chtype URCorner;
  chtype LLCorner;
  chtype LRCorner;
  chtype HLine;
  chtype VLine;
  chtype UArrow;
  chtype DArrow;
  chtype LArrow;
  chtype RArrow;
  ch_type Unknown;
  ch_type Mine;
  ch_type ShowMine;
  ch_type Space;
  ch_type Mark;
  ch_type FalseMark;
  ch_type ShowFalseMark;
  ch_type Bombed;
  ch_type CursorLeft;
  ch_type CursorRight;
} DrawChars;

/* This is the struct containing all the relevant game information. */
typedef struct game_stats_s {
  int height;
  int width;
  int percent;
  unsigned int mines;
  unsigned int MarkedMines;
  unsigned int BadMarkedMines;
  alert_t alert;
  int theme;
  int CursorX, CursorY;
  int LargeBoardX, LargeBoardY;
  game_status_t Status;
  int FocusX, FocusY;
  unsigned int Time;
  unsigned char* Field;
  WINDOW* Border;
  WINDOW* Board;
} game_stats_t;

extern DrawChars CharSet;

/* These are the functions defined in main.c */
void game_exit(int result);

/* These are the functions defined in files.c */
int config_load_user(game_stats_t* game);
int config_load_global(game_stats_t* game);
int config_save(game_stats_t* game);

/* These are the functions defined in drawing.c */
void curses_init();
void draw_title();
void draw_splash();
int draw_board(game_stats_t* game);
void cursor_draw(game_stats_t* game);
void cursor_undraw(game_stats_t* game);
int board_pan(game_stats_t* game);
int cursor_center(game_stats_t* game);
int cursor_center_y(game_stats_t* game);
int cursor_center_x(game_stats_t* game);

/* These are the functions defined in game.c */
int check_width(int value);
int check_height(int value);
int check_theme(int value);
int check_percent(int value);
int game_init(game_stats_t* game);
int game_new(game_stats_t* game);
int game_create_board(game_stats_t* game);
int game_args(game_stats_t* game, int argc, char** argv);
int game_set_mines(game_stats_t* game);
void game_set_mine(game_stats_t* game, int x, int y, unsigned char value);
unsigned char game_get_mine(game_stats_t* game, int x, int y);

/* These are the functions defined in play.c */
void dialog_lose();
void dialog_win();
void dialog_abort();
int game_input(game_stats_t* game);

/* These are the functions defined in themes.c */
void theme_set(game_stats_t* game);

/* These are defined in log.c */
typedef struct _log {
  char *line;
  struct _log *next;
} log_t;

void log_splash(int ncols, int y, int x);
void log_init(game_stats_t *game);
void log_set_alert(game_stats_t *game);
void log_resize();
void log_refresh();
void log_message(const char *format, ...);
void log_status(const char *format, ...);
void log_error(const char *format, ...);
log_t* logs();
void log_close();

/* These are defined in pause.c */
void pause_display();

/* These are the functions defined in stats.c */
int stats_init();
void stats_display(game_stats_t *Game);
void stats_resize();
int stats_refresh();
void stats_close();

/* These are the functions described in utils.c */
int xcolor(int r, int g, int b, int fallback);
int xwaddch(WINDOW* win, DrawChars *CharSet, ch_type ch);
void* xmalloc(size_t num);
char* xgetcwd(char *buf, size_t size);
DIR* xopendir(const char *buf);
int xexists(const char *path);
static inline int xmax(int v1, int v2) {
  return (v2 > v1 ? v2 : v1);
}
static inline int xmin(int v1, int v2) {
  return (v2 < v1 ? v2 : v1);
}
const char *xtrunc(int length, const char *line);
void timer_start();
void timer_stop();

/* This is in bests.c */
typedef struct bests_entry_s {
  unsigned int area;
  unsigned int mines;
  unsigned int time;

  uid_t user;
  time_t date;
} bests_entry_t;

typedef struct bests_table_s {
  /* the array of entries from the file, with one more in it. */
  bests_entry_t *entries;

  /* the number of entries in the descriptor */
  int numents;
  int alloced; // Number of allocated entries.

  int sorted; // Is the table sorted?
} bests_table_t;

void bests_update(game_stats_t *game);
bests_table_t* bests_load();
void bests_free(bests_table_t* table);
void bests_print();

/* These are the functions defined in clear.c */
void game_clear(game_stats_t *Game);
void game_super_clear(game_stats_t *Game);
int game_find_nearest(game_stats_t *Game);
int game_find_nearest_bad(game_stats_t *Game);

/* functions defined in sl.c for save/load games */
void game_save(game_stats_t* Game);
int game_load(game_stats_t* Game);

/* functions defined in tick.c */
extern volatile unsigned int g_tick;
void sighandler(int signo);

/* functions in image.c */
void game_save_image(game_stats_t* game);

/* functions in xdg.c */
char *xdg_config_home();
char *xdg_data_home();

#endif /* __SWEEP_H__ */
