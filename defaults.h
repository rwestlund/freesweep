/*********************************************************************
* $Id: defaults.h,v 1.3 1999-02-18 05:25:04 hartmann Exp $
*********************************************************************/

#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

#define GLOBAL_PREFS_FILE "/usr6/hartmann/sweep/sweeprc"

#define DEFAULT_HEIGHT 15
#define DEFAULT_WIDTH 15
#define DEFAULT_PERCENT 20
#define DEFAULT_COLOR 1

#define DEFAULT_NUMMINES 0
#define DEFAULT_FASTSTART 0
#define DEFAULT_ALERT 0
#define VI_KEYS

/* Don't use Linedraw by default if not using ncurses. */
#ifdef HAVE_LIBNCURSES
#define DEFAULT_LINEDRAW 1
#else /* HAVE_LIBNCURSES */
#define DEFAULT_LINEDRAW 0
#endif /* HAVE_LIBNCURSES */

#endif /* __DEFAULTS_H__ */
