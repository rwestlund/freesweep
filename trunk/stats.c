#include "sweep.h"

static WINDOW* StatsFrame;
static WINDOW* StatsWin;

static void ClearStats(void);

int InitStatsWin(void)
{
	if ((StatsFrame=newwin(7,INFO_W,6,(COLS-INFO_W)))==NULL)
	{ 
		return 1;
	}
	else if ((StatsWin=derwin(StatsFrame,5,19,1,1))==NULL)
	{
		return 1;
	}
	wborder(StatsFrame,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,
		CharSet.ULCorner, CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner); 
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
	return ((wborder(StatsFrame,CharSet.VLine,CharSet.VLine,CharSet.HLine,
		CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,
		CharSet.LRCorner)!=OK)?ERR:wnoutrefresh(StatsFrame));
}

void PrintStats(GameStats *Game)
{
	float percentage=0.0;
	percentage=100 * ((Game->MarkedMines + Game->BadMarkedMines) / (Game->NumMines * 1.0));

	ClearStats();

	mvwprintw(StatsWin, 0, 0, "Time: %d", Game->Time);
	mvwprintw(StatsWin, 1, 0, "Loc: %d, %d", Game->CursorX, Game->CursorY);
	mvwprintw(StatsWin, 2, 0, "Mines: %d", Game->NumMines);
	mvwprintw(StatsWin, 3, 0, "Marks:");
	mvwprintw(StatsWin, 4, 0, "Percentage:");
	
	if ( percentage > 100.0)
	{
		wstandout(StatsWin);
		mvwprintw(StatsWin, 3, 7, "%d", Game->MarkedMines + Game->BadMarkedMines);
		mvwprintw(StatsWin, 4, 13, "%3.2f%%", percentage);
		wstandend(StatsWin);
	}
	else
	{
		mvwprintw(StatsWin, 3, 7, "%d", Game->MarkedMines + Game->BadMarkedMines);
		mvwprintw(StatsWin, 4, 13, "%3.2f%%", percentage);
	}

	wnoutrefresh(StatsWin);
	move(0,0);
	noutrefresh();
	return;
}
