/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: utils.c,v 1.8 2003-10-11 20:50:50 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"
#include <unistd.h>

/******************************************************************************
 * Color conversion code borrowed and modified from the tmux project.
 *
 * Copyright (c) 2008 Nicholas Marriott <nicholas.marriott@gmail.com>
 * Copyright (c) 2016 Avi Halachmi <avihpit@yahoo.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

static int ColorDist(int R, int G, int B, int r, int g, int b)
{
        return ((R - r) * (R - r) + (G - g) * (G - g) + (B - b) * (B - b));
}

static int Color6Cube(int value)
{
        if (value < 48) return 0;
        if (value < 114) return 1;
        return ((value - 35) / 40);
}

static int ColorRGB(int R, int G, int B)
{
        /* Convert an RGB triplet to the xterm(1) 256 colour palette.
         *
         * xterm provides a 6x6x6 colour cube (16 - 231) and 24 greys (232 - 255). We
         * map our RGB colour to the closest in the cube, also work out the closest
         * grey, and use the nearest of the two.
         *
         * Note that the xterm has much lower resolution for darker colours (they are
         * not evenly spread out), so our 6 levels are not evenly spread: 0x0, 0x5f
         * (95), 0x87 (135), 0xaf (175), 0xd7 (215) and 0xff (255). Greys are more
         * evenly spread (8, 18, 28 ... 238).
         */
        static const int q2c[6] = {0x00,0x5f,0x87,0xaf,0xd7,0xff};
        int qr,qg,qb,cr,cg,cb,d,idx;
        int grey_avg,grey_idx,grey;

        /* Map RGB to 6x6x6 cube color space. */
        qr = Color6Cube(R); cr = q2c[qr];
        qb = Color6Cube(B); cb = q2c[qb];
        qg = Color6Cube(G); cg = q2c[qg];

        /* If we hit the color exactly return early. */
        if (cr == R && cb == B && cg == G)
                return ((16 + (36 * qr) + (16 * qg) + qb));

        /* Work out the closest grey (average of RGB). */
        grey_avg = (R + G + B) / 3;
        if (grey_avg > 238)
                grey_idx = 23;
        else
                grey_idx = (grey_avg - 3) / 10;
        grey = 8 + (10 * grey_idx);

        /* Is grey or 6x6x6 colour closest? */
        d = ColorDist(cr, cg, cb, R, G, B);
        if (ColorDist(grey, grey, grey, R, G, B) < d)
                idx = 232 + grey_idx;
        else
                idx = 16 + (36 * qr) + (6 * qg) + qb;
        return idx;
}

int xcolor(int r, int g, int b, int fallback)
{
        return (COLORS >= 256 ? ColorRGB(r,g,b) : fallback);
}

int xwaddch(WINDOW* win, DrawChars *CharSet, ch_type ch)
{
        switch (CharSet->ch_flag)
        {
        case CH_TYPE:
                return waddch(win, ch.ch);
        case UNI_TYPE:
                return waddstr(win, ch.str);
        }
        return ERR;
}

void* xmalloc(size_t num)
{
        void *vec = NULL;

        vec = (void*)malloc(sizeof(unsigned char) * num);

        if (vec == NULL)
        {
                SweepError("Out of Memory. Sorry");
                exit(EXIT_FAILURE);
        }

        return vec;
}

#ifndef HAVE_STRDUP
char *strdup(char *s)
{
        char *c = NULL;

        c = (char*)xmalloc(strlen(s) + 1);

#if defined(HAVE_STRNCPY)
        strncpy(c, s, strlen(s) + 1);
#else
        strcpy(c, s);
#endif

        return(c);

}
#endif

char* xgetcwd(char *buf, size_t size)
{
        char *path = NULL;

        path = getcwd(buf, size);

        if (path == NULL)
        {
                SweepError("Could not get current working directory.");
                exit(EXIT_FAILURE);
        }

        return path;
}

DIR* xopendir(const char *path)
{
        DIR *dirent = NULL;

        dirent = opendir(path);

        if (dirent == NULL)
        {
                return NULL;
        }

        return dirent;
}

int xexists(const char *path)
{
        return (access(path, F_OK) != -1);
}

/* start and stop the timer */
void StartTimer(void)
{
        signal(SIGALRM, sighandler);
        alarm(1);
}

/* stop the timer */
void StopTimer(void)
{
        signal(SIGALRM, SIG_IGN);
}
