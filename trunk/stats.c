#include "sweep.h"

static WINDOW* StatsFrame;
static WINDOW* StatsWin;

static void ClearStats(void);

int InitStatsWin(void)
{
	if ((StatsFrame=newwin(6,INFO_W,6,(COLS-INFO_W)))==NULL)
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
	werase(StatsWin);
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
	ClearStats();

	mvwprintw(StatsWin, 0, 0, "Time: %d", Game->Time);
	mvwprintw(StatsWin, 1, 0, "Loc: %d, %d", Game->CursorX, Game->CursorY);
	mvwprintw(StatsWin, 2, 0, "Mines: %d", Game->NumMines);
	mvwprintw(StatsWin, 3, 0, "Marks: %d", Game->MarkedMines + Game->BadMarkedMines);

	wnoutrefresh(StatsWin);
	move(0,0);
	noutrefresh();
	return;
}
