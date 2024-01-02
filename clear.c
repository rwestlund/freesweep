/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: clear.c,v 1.11 2002-07-12 07:06:44 hartmann Exp $
*                                                                     *
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "sweep.h"


static void InsertChildren(struct Mark **ht, GameStats *Game, int x, int y);
static int CalcSquareNumber(GameStats *Game, int xx, int yy);
static int ExposeSquare(GameStats *Game, int xx, int yy, int total);
static unsigned char* InitLookupTable(void);
static struct Mark* NewMark(int x, int y);
static int SuperCount(GameStats* Game);

/* I should clean these up */
int g_num = 0;
unsigned char *g_table = NULL;
int g_table_w;
int g_table_h = 0;
struct Mark *g_mlist = NULL;

void Clear(GameStats *Game)
{
        struct Mark **ht = NULL;
        int total;
        int x, y;

        g_num = 0;
        g_table = InitLookupTable();

        /* give it the first candidate */
        InsertMark(ht, Game->CursorX, Game->CursorY);

        /* This is data recursion */
        while(DeleteRandomMark(ht, &x, &y) == TRUE)
        {
                total = CalcSquareNumber(Game, x, y);
                if (ExposeSquare(Game, x, y, total) == EMPTY)
                {
                        InsertChildren(ht, Game, x, y);
                }
        }

        free (g_table);
}

/* grab the eight or so children around the mark and insert them into the
 * hash table */
void InsertChildren(struct Mark **ht, GameStats *Game, int x, int y)
{
        unsigned char retval;

        /* Check the above squares */
        if (y-1 >= 0)
        {
                /* Check upper-left */
                if (x-1 >= 0)
                {
                        GetMine(x-1, y-1, retval);
                        if (retval == UNKNOWN)
                        {
                                InsertMark(ht, x-1, y-1);
                        }
                }

                /* Check directly above */
                GetMine(x, y-1, retval);
                if (retval == UNKNOWN)
                {
                        InsertMark(ht, x, y-1);
                }

                /* Check upper-right */
                if (x+1 < Game->Width)
                {
                        GetMine(x+1, y-1, retval);
                        if (retval == UNKNOWN)
                        {
                                InsertMark(ht, x+1, y-1);
                        }
                }
        }

        if (x-1 >= 0)
        {
                GetMine(x-1, y, retval);
                if (retval == UNKNOWN)
                {
                        InsertMark(ht, x-1, y);
                }
        }

        if (x+1 < Game->Width)
        {
                GetMine(x+1, y, retval);
                if (retval == UNKNOWN)
                {
                        InsertMark(ht, x+1, y);
                }
        }

        /* Check the below squares */
        if (y+1 < Game->Height)
        {
                /* Check lower-left */
                if (x-1 >= 0)
                {
                        GetMine(x-1, y+1, retval);
                        if (retval == UNKNOWN)
                        {
                                InsertMark(ht, x-1, y+1);
                        }
                }

                /* Check directly below */
                GetMine(x, y+1, retval);
                if (retval == UNKNOWN)
                {
                        InsertMark(ht, x, y+1);
                }

                /* Check lower-right */
                if (x+1 < Game->Width)
                {
                        GetMine(x+1, y+1, retval);
                        if (retval == UNKNOWN)
                        {
                                InsertMark(ht, x+1, y+1);
                        }
                }
        }
}

/* determine the number of a particular square */
int CalcSquareNumber(GameStats *Game, int x, int y)
{
        unsigned char SquareVal;
        int Total=0;

        GetMine(x,y,SquareVal);
        if (SquareVal != UNKNOWN)
        {
                return SquareVal;
        }

        /* Check the above squares */
        if (y-1 >= 0)
        {
                /* Check upper-left */
                if (x-1 >= 0)
                {
                        GetMine(x-1,y-1,SquareVal);
                        if ((SquareVal == MINE)||(SquareVal == MARKED)||(SquareVal == DETONATED))
                        {
                                Total++;
                        }
                }

                /* Check directly above */
                GetMine(x,y-1,SquareVal);
                if ((SquareVal == MINE)||(SquareVal == MARKED)||(SquareVal == DETONATED))
                {
                        Total++;
                }

                /* Check upper-right */
                if (x+1 < Game->Width)
                {
                        GetMine(x+1,y-1,SquareVal);
                        if ((SquareVal == MINE)||(SquareVal == MARKED)||(SquareVal == DETONATED))
                        {
                                Total++;
                        }
                }
        }

        if (x-1 >= 0)
        {
                GetMine(x-1,y,SquareVal);
                if ((SquareVal == MINE)||(SquareVal == MARKED)||(SquareVal == DETONATED))
                {
                        Total++;
                }
        }

        if (x+1 < Game->Width)
        {
                GetMine(x+1,y,SquareVal);
                if ((SquareVal == MINE)||(SquareVal == MARKED)||(SquareVal == DETONATED))
                {
                        Total++;
                }
        }

        /* Check the below squares */
        if (y+1 < Game->Height)
        {
                /* Check lower-left */
                if (x-1 >= 0)
                {
                        GetMine(x-1,y+1,SquareVal);
                        if ((SquareVal == MINE)||(SquareVal == MARKED)||(SquareVal == DETONATED))
                        {
                                Total++;
                        }
                }

                /* Check directly below */
                GetMine(x,y+1,SquareVal);
                if ((SquareVal == MINE)||(SquareVal == MARKED)||(SquareVal == DETONATED))
                {
                        Total++;
                }

                /* Check lower-right */
                if (x+1 < Game->Width)
                {
                        GetMine(x+1,y+1,SquareVal);
                        if ((SquareVal == MINE)||(SquareVal == MARKED)||(SquareVal == DETONATED))
                        {
                                Total++;
                        }
                }
        }

        return Total;
}

/* fill in what a square is number wise */
int ExposeSquare(GameStats *Game, int x, int y, int total)
{
        SetMine(x, y, total==0?EMPTY:total);

        return (total==0?EMPTY:total);
}

unsigned char* InitLookupTable(void)
{
        int width, height;
        unsigned char *m = NULL;

        width = MAX_W / 8;
        if (MAX_W%8)
        {
                width++;
        }

        height = MAX_H;
        if (MAX_W%8)
        {
                height++;
        }

        m = (unsigned char*)malloc(sizeof(unsigned char) * width * height);

        if (m == NULL)
        {
                SweepError("Out of memory. Aborting...");
                exit(EXIT_FAILURE);
        }

        g_table_w = width;
        g_table_h = height;

        memset(m, 0, width*height);

        return (m);
}

/* if x,y exists, do nothing, otherwise, make a mark and insert it */
void InsertMark(struct Mark **ht, int x, int y)
{
        struct Mark *m = NULL;

        if (LOOKUP(g_table, x, y))
        {
                return;
        }
        SET(g_table, x, y);
        g_num++;

        m = NewMark(x, y);

        m->next = g_mlist;
        g_mlist = m;
}

/* find one in the table and deal with it */
char DeleteRandomMark(struct Mark **ht, int *xx, int *yy)
{
        struct Mark *m = NULL;

        m = g_mlist;
        if (m == NULL)
        {
                return FALSE;
        }

        g_mlist = g_mlist->next;

        *xx = m->x;
        *yy = m->y;
        UNSET(g_table, *xx, *yy);

        free(m);

        return TRUE;
}

/* make a new mark initialized nicely */
struct Mark* NewMark(int x, int y)
{
        struct Mark *m = NULL;

        m = (struct Mark*)malloc(sizeof(struct Mark) * 1);

        if (m == NULL)
        {
                SweepError("Out of memory! Sorry.");
                exit(EXIT_FAILURE);
        }

        m->x = x;
        m->y = y;
        m->next = NULL;

        return (m);
}

void SuperClear(GameStats *Game)
{
        int val;
        struct Mark **ht = NULL;
        int total;
        int x, y;

        x = Game->CursorX;
        y = Game->CursorY;
        val = SuperCount(Game);

        switch(val)
        {
                /* they marked incorrectly, and must now pay the price */
                case DIE:
/*                      Boom();*/
                        Game->Status = LOSE;
                        SetMine(x,y,DETONATED);
                        break;
                /* they marked correctly, and I can expand stuff */
                case SUPERCLICK:
                        g_num = 0;
                        g_table = InitLookupTable();

                        /* insert all the ones around me */
                        InsertChildren(ht, Game, x, y);

                        /* This is data recursion */
                        while(DeleteRandomMark(ht, &x, &y) == TRUE)
                        {
                                total = CalcSquareNumber(Game, x, y);
                                if (ExposeSquare(Game, x, y, total) == EMPTY)
                                {
                                        InsertChildren(ht, Game, x, y);
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

/* calculate if I should superclick, die, or do nothing. */
int SuperCount(GameStats* Game)
{
        unsigned char retval;
        int MinesFound=0,BadFound=0;
        int x, y;

        x = Game->CursorX;
        y = Game->CursorY;

        /* Check the above squares */
        if (y-1 >= 0)
        {
                /* Check upper-left */
                if (x-1 >= 0)
                {
                        GetMine(x-1, y-1, retval);
                        switch (retval)
                        {
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
                GetMine(x, y-1, retval);
                switch (retval)
                {
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
                if (x+1 < Game->Width)
                {
                        GetMine(x+1, y-1, retval);
                        switch (retval)
                        {
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

        if (x-1 >= 0)
        {
                GetMine(x-1, y, retval);
                switch (retval)
                {
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

        if (x+1 < Game->Width)
        {
                GetMine(x+1, y, retval);
                switch (retval)
                {
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
        if (y+1 < Game->Height)
        {
                /* Check lower-left */
                if (x-1 >= 0)
                {
                        GetMine(x-1, y+1, retval);
                        switch (retval)
                        {
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
                GetMine(x, y+1, retval);
                switch (retval)
                {
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
                if (x+1 < Game->Width)
                {
                        GetMine(x+1, y+1, retval);
                        switch (retval)
                        {
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

        /* some incorrectly marked mines */
        if (BadFound != 0)
        {
                return DIE;
        }
        /* some correctly marked, some not marked mines */
        else if (MinesFound != 0 && BadFound == 0)
        {
                return DONOTHING;
        }
        /* all mines correctly marked */
        else
        {
                return SUPERCLICK;
        }
}

#define TEST(a,b) if(a>=0 && a< Game->Height && b>=0 && b<Game->Width ) { GetMine(x, y, square) ; if (( square == MINE ) || ( square == UNKNOWN )) { Game->CursorX=a ; Game->CursorY=b ; return 0 ; } }

int FindNearest(GameStats *Game)
{
        int x, y;
        int minx, miny, minscore, score;
        unsigned char square;

        x = Game->CursorX;
        y = Game->CursorY;

        GetMine(x, y, square);
        if (( square == MINE ) || ( square == UNKNOWN ))
        {
                /* We're *on* an unmarked square already! */
                return 0;
        }

        /* This should be really, really big. */
/*      minscore = (  Game->Width ) * ( Game->Width ) * ( Game->Height ) * ( Game->Height ));*/
/*      minscore = MAX_INT;*/
        minscore = 0x7ffffff;
        minx = Game->CursorX;
        miny = Game->CursorY;

        for ( x = 0 ; x < Game->Width ; x++ ) {
                for ( y = 0 ; y < Game->Height ; y++ ) {
                        GetMine(x, y, square);
                        if (( square == MINE ) || ( square == UNKNOWN ))
                        {
                                score = (( Game->CursorX - x ) * ( Game->CursorX - x )) + (( Game->CursorY - y ) * ( Game->CursorY - y ));
                                if ( score < minscore )
                                {
                                        minscore = score;
                                        minx = x;
                                        miny = y;
                                }
                        }
                }
        }

        Game->CursorX = minx;
        Game->CursorY = miny;

#ifdef DEBUG_LOG
        fprintf(DebugLog, "Minimum score of %d found at %d,%d\n", minscore, minx, miny);
        fflush(DebugLog);
#endif /* DEBUG_LOG */

        return 0;
}

int FindNearestBad(GameStats *Game)
{
        int x, y;
        int minx, miny, minscore, score;
        unsigned char square;

        x = Game->CursorX;
        y = Game->CursorY;

        GetMine(x, y, square);
        if (( square == BAD_MARK ) || ( Game->BadMarkedMines == 0 ))
        {
                /* We're done here. */
                return 0;
        }

        /* This should be really, really big. */
/*      minscore = (  Game->Width ) * ( Game->Width ) * ( Game->Height ) * ( Game->Height ));*/
/*      minscore = MAX_INT;*/
        minscore = 0x7ffffff;
        minx = Game->CursorX;
        miny = Game->CursorY;

        for ( x = 0 ; x < Game->Width ; x++ ) {
                for ( y = 0 ; y < Game->Height ; y++ ) {
                        GetMine(x, y, square);
                        if ( square == BAD_MARK )
                        {
                                score = (( Game->CursorX - x ) * ( Game->CursorX - x )) + (( Game->CursorY - y ) * ( Game->CursorY - y ));
                                if ( score < minscore )
                                {
                                        minscore = score;
                                        minx = x;
                                        miny = y;
                                }
                        }
                }
        }

        Game->CursorX = minx;
        Game->CursorY = miny;

#ifdef DEBUG_LOG
        fprintf(DebugLog, "Bad mark: minimum score of %d found at %d,%d\n", minscore, minx, miny);
        fflush(DebugLog);
#endif /* DEBUG_LOG */

        return 0;
}
