#include 'sweep.h"

void PrintGPL()
{
	WINDOW* GPLWin;
	int Input=0, CurrentLine=0;
	char* Messages[]=
	{
		"This will be the GPL."
	};

	if ((GPLWin=newwin(0,0,0,0))==NULL)
	{
		perror("PrintGPL::newwin");
		exit(EXIT_FAILURE);
	}
	wborder(GPLWin,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark);
	mvwprintw(GPLWin,1,2,"Time out - The GNU General Public License");
	mvwhline(GPLWin,2,1,CharSet.HLine,COLS-2);

	/* Be sure to update this to account for all of the error messages. */
	for (CurrentLine=0;CurrentLine<11;CurrentLine++)
	{
		mvwprintw(GPLWin,CurrentLine+4,6,Messages[CurrentLine]);
	}

	mvwprintw(GPLWin,LINES-1,1,"--Press \'q\' to quit, any other key to continue.--");
	wmove(GPLWin,0,0);
	wrefresh(GPLWin);
	Input=wgetch(GPLWin);
	if ((Input=='q')||(Input=='Q'))
	{
		clear();
		refresh();
		endwin();
		exit(EXIT_SUCCESS);
	}
	else
	{
		wclear(GPLWin);
		delwin(GPLWin);
		clear();
		noutrefresh();
	}
	return;
}

