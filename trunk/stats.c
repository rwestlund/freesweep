#include "sweep.h"

static WINDOW* StatsFrame;
static WINDOW* StatsWin;

static void ClearStats(void);

int InitStatsWin(void)
{
	if ((StatsFrame=newwin(6,21,5,(COLS-INFO_W)))==NULL)
	{ 
		return 1;
	}
	else if ((StatsWin=derwin(StatsFrame,4,19,1,1))==NULL)
	{
		return 1;
	}
	wborder(StatsFrame,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,
		CharSet.ULCorner, CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner); 
	wnoutrefresh(StatsWin);
	wnoutrefresh(StatsFrame);
	return 0;
}

void ClearStats()
{
	/* do a line by line clear */

	wmove(StatsWin,0,0);
	wclrtoeol(StatsWin);

	wmove(StatsWin,1,0);
	wclrtoeol(StatsWin);

	wmove(StatsWin,2,0);
	wclrtoeol(StatsWin);

	wmove(StatsWin,3,0);
	wclrtoeol(StatsWin);

	wnoutrefresh(StatsWin);
	move(0,0);
	noutrefresh();
	return;
}

int RedrawStatsWin(void)
{
	return ((wborder(StatsFrame,CharSet.VLine,CharSet.VLine,CharSet.HLine,
		CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,
		CharSet.LRCorner)!=OK)?ERR:wnoutrefresh(StatsFrame));
}

void PrintStats(GameStats *Game)
{
	char buf[MAX_LINE];

	ClearStats();

	sprintf(buf, "Time: %d", Game->Time);
	mvwprintw(StatsWin, 0, 0, buf);
	sprintf(buf, "Loc: %d, %d", Game->CursorX, Game->CursorY);
	mvwprintw(StatsWin, 1, 0, buf);
	sprintf(buf, "Mines: %d", Game->NumMines);
	mvwprintw(StatsWin, 2, 0, buf);
	sprintf(buf, "Marks: %d", Game->MarkedMines + Game->BadMarkedMines);
	mvwprintw(StatsWin, 3, 0, buf);

	wnoutrefresh(StatsWin);
	move(0,0);
	noutrefresh();
	return;
}
