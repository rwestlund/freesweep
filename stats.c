/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: stats.c,v 1.11 2002-12-20 08:44:53 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"

static WINDOW* StatsFrame;
static WINDOW* StatsWin;

static void ClearStats(void);

int InitStatsWin(void)
{
        if ((StatsFrame=newwin(LINES-7,INFO_W,6,(COLS-INFO_W)))==NULL)
        {
                return 1;
        }
        else if ((StatsWin=derwin(StatsFrame,5,19,0,1))==NULL)
        {
                return 1;
        }
        wbkgdset(StatsFrame, ' ' | COLOR_PAIR(CLR_STATBAR));
        wbkgdset(StatsWin, ' ' | COLOR_PAIR(CLR_STATBAR));
        wclear(StatsFrame);
        wnoutrefresh(StatsFrame);
        wnoutrefresh(StatsWin);
        return 0;
}

void ClearStats()
{
        werase(StatsWin);
        wnoutrefresh(StatsWin);
        move(0,0);
        noutrefresh();
        return;
}

int RedrawStatsWin(void)
{
  wclear(StatsFrame);
  return wnoutrefresh(StatsFrame);
        return ((wborder(StatsFrame,CharSet.VLine,CharSet.VLine,CharSet.HLine,
                CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,
                CharSet.LRCorner)!=OK)?ERR:wnoutrefresh(StatsFrame));
}

void PrintStats(GameStats *Game)
{
        float percentage=0.0;
        percentage=100 * ((Game->MarkedMines + Game->BadMarkedMines) / (Game->NumMines * 1.0));

        ClearStats();

/*      mvwprintw(StatsWin, 0, 0, "Time: %ds", Game->Time);*/
        /*
                Put better clock here.
        */
        if ( Game->Time >= 86400 )
        {
                mvwprintw(StatsWin, 0, 0, "Time: %d:%02d:%02d:%02ds", Game->Time / 86400, ( Game->Time % 86400 ) / 3600, ( Game->Time % 3600 ) / 60 , Game->Time % 60 );
        }
        else if ( Game->Time >= 3600 )
        {
                mvwprintw(StatsWin, 0, 0, "Time: %d:%02d:%02ds", Game->Time / 3600, ( Game->Time % 3600 ) / 60 , Game->Time % 60 );
        }
        else if ( Game->Time >= 60 )
        {
                mvwprintw(StatsWin, 0, 0, "Time: %d:%02ds", Game->Time / 60 , Game->Time % 60 );
        }
        else
        {
                mvwprintw(StatsWin, 0, 0, "Time: %ds", Game->Time);
        }




        mvwprintw(StatsWin, 1, 0, "Loc: %d, %d", Game->CursorX, Game->CursorY);
        mvwprintw(StatsWin, 2, 0, "Mines: %d", Game->NumMines);
        mvwprintw(StatsWin, 3, 0, "Marks:");
        mvwprintw(StatsWin, 4, 0, "Percentage:");

        if ( percentage > 100.0)
        {
                if (Game->Color != 0 && (has_colors() == TRUE))
                {
                        wcolor_set(StatsWin,6,NULL);
                }
                else
                {
                        wstandout(StatsWin);
                }
                mvwprintw(StatsWin, 3, 7, "%d", Game->MarkedMines + Game->BadMarkedMines);
                mvwprintw(StatsWin, 4, 12, "%3.2f%%", percentage);
                if (Game->Color != 0 && (has_colors() == TRUE))
                {
                        wcolor_set(StatsWin,1,NULL);
                }
                else
                {
                        wstandend(StatsWin);
                }
        }
        else
        {
                mvwprintw(StatsWin, 3, 7, "%d", Game->MarkedMines + Game->BadMarkedMines);
                mvwprintw(StatsWin, 4, 12, "%3.2f%%", percentage);
        }

        wnoutrefresh(StatsWin);
        move(0,0);
        noutrefresh();
        return;
}
